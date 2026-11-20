#!/usr/bin/env ruby
# frozen_string_literal: true

# Automated Open Watcom DOS/32A End-to-End Verification Suite via Vagrant + Bento
#
# Verifies:
#   1. Open Watcom `<OWRELROOT>` staging hygiene:
#      - DOS extender (`dos32a.exe`, `stub32a.exe`, `stub32c.exe`) and DOS 32-bit
#        tools (`sb.exe`, `sc.exe`, `sd.exe`, `ss.exe`) exist ONLY in `binw/`.
#      - Host `sver` binary runs natively on the build host and inspects all
#        built DOS/32A binaries cleanly.
#      - Deterministic 512-byte (`e_cparhdr == 32`, `e_lfarlc == 0x40`) MZ header
#        alignment on `dos32a.exe` for MS-DOS 1.25 compatibility.
#   2. End-to-end 32-bit C compilation & linking via `*bwcc386` + `*bwlink`
#      (`build/bwlink.lnk`) using `system dos32a` (bound unified `dos32a.exe`)
#      and `system stub32a` (`stub32a.exe` + external `dos32a.exe`).
#   3. Real Vagrant (`bento/msdos-1.25`, `bento/msdos-2.0`, `bento/msdos-4.0`)
#      protected-mode execution across MS-DOS 1.25, MS-DOS 2.00, and MS-DOS 4.00.

require "fileutils"
require "rbconfig"
require "socket"
require "timeout"
require "tmpdir"

TEST_DIR = __dir__
OW_ROOT  = File.expand_path("../../..", TEST_DIR)
REL_ROOT = ENV["OWRELROOT"] || File.join(OW_ROOT, "rel")
BINW_DIR = File.join(REL_ROOT, "binw")
BINBUILD = File.join(OW_ROOT, "build", "binbuild")

begin
  require "bento/vagrant_msdos"
rescue LoadError
  [
    File.expand_path("../../../../bento/lib", TEST_DIR),
    File.expand_path("../../../bento/lib", TEST_DIR)
  ].each do |bento_lib|
    $LOAD_PATH.unshift(bento_lib) if File.directory?(bento_lib) && !$LOAD_PATH.include?(bento_lib)
  end
  require "bento/vagrant_msdos"
end

$stdout.sync = true

def assert_true(cond, msg)
  raise "ASSERTION FAILED: #{msg}" unless cond
end

def port_open?(host, port)
  Timeout.timeout(0.15) do
    sock = TCPSocket.new(host, port)
    sock.close
    true
  end
rescue StandardError
  false
end

def pick_free_port(default_port)
  port = default_port
  port += 100 while port_open?("127.0.0.1", port)
  port
end

def read_mz_header(path)
  raw = File.binread(path, 64)
  assert_true(raw && raw.bytesize >= 64, "#{path}: shorter than 64 bytes")
  magic = raw[0, 2]
  assert_true(magic == "MZ" || magic == "ZM", "#{path}: invalid MZ magic #{magic.inspect}")
  words = raw[0, 32].unpack("v16")
  e_lfanew = raw[60, 4].unpack1("V")
  {
    e_cparhdr: words[4],
    header_bytes: words[4] * 16,
    e_lfarlc: words[12],
    e_lfanew: e_lfanew
  }
end

def host_rel_bin_dir(rel_root)
  host_os  = RbConfig::CONFIG["host_os"].to_s.downcase
  host_cpu = RbConfig::CONFIG["host_cpu"].to_s.downcase
  is_arm   = host_cpu.include?("arm") || host_cpu.include?("aarch64")

  candidates =
    if host_os.include?("darwin")
      is_arm ? %w[armo64 bino64] : %w[bino64 armo64]
    elsif host_os =~ /mswin|mingw|cygwin/
      %w[binnt64 binnt]
    elsif host_os.include?("bsd")
      is_arm ? %w[armb64] : %w[binb64]
    else
      is_arm ? %w[arml64 arml] : %w[binl64 binl]
    end

  found = candidates.find { |sub| File.directory?(File.join(rel_root, sub)) }
  raise "Could not locate host bin directory in #{rel_root}" unless found

  File.join(rel_root, found)
end

def open_msdos_communicator(serial_port, dos_version)
  cfg = Bento::VagrantMsdos::Config.new
  cfg.host = ENV["MSDOS_SERIAL_HOST"] || "127.0.0.1"
  cfg.port = serial_port
  cfg.dos_version = dos_version
  cfg.finalize!

  vm_s   = Struct.new(:communicator, :guest).new(:msdos, :msdos)
  conf_s = Struct.new(:vm, :msdos).new(vm_s, cfg)
  mach_s = Struct.new(:config, :data_dir).new(conf_s, nil)
  Bento::VagrantMsdos::Communicator.new(mach_s)
end

def run_dos_cmd(comm, cmd)
  out = +""
  comm.execute(cmd, error_check: false) { |_type, data| out << data }
  out
end

puts "==> [1/4] Auditing <OWRELROOT> staging and MZ header invariants..."
%w[dos32a.exe stub32a.exe stub32c.exe sb.exe sc.exe sd.exe ss.exe sver.exe license.d32 readme.d32].each do |f|
  assert_true(File.file?(File.join(BINW_DIR, f)), "Missing expected #{f} in #{BINW_DIR}")
end
%w[mzalign.exe dos32a.stb dos32a_stock.exe].each do |forbidden|
  assert_true(!File.exist?(File.join(BINW_DIR, forbidden)), "Forbidden artifact #{forbidden} present in #{BINW_DIR}")
end

non_binw_dirs = %w[binp binnt binnt64 axpnt binl binl64 arml arml64 binb64 armb64 bino64 armo armo64]
non_binw_dirs.each do |sub|
  dir = File.join(REL_ROOT, sub)
  next unless File.directory?(dir)

  %w[dos32a.exe dos32a.stb stub32a.exe stub32c.exe sb.exe sc.exe sd.exe ss.exe].each do |ext_file|
    assert_true(!File.exist?(File.join(dir, ext_file)), "Extender/DOS binary #{ext_file} leaked into #{dir}")
  end
end

d32_hdr = read_mz_header(File.join(BINW_DIR, "dos32a.exe"))
assert_true(d32_hdr[:header_bytes] == 512, "dos32a.exe header_bytes=#{d32_hdr[:header_bytes]} (expected 512)")
assert_true(d32_hdr[:e_lfarlc] == 0x40, "dos32a.exe e_lfarlc=0x#{d32_hdr[:e_lfarlc].to_s(16)} (expected 0x40)")

host_bin  = host_rel_bin_dir(REL_ROOT)
host_sver = [File.join(host_bin, "sver"), File.join(host_bin, "sver.exe")].find { |p| File.executable?(p) }
assert_true(!host_sver.nil?, "Native host sver executable not found in #{host_bin}")

sver_d32 = `#{host_sver} #{File.join(BINW_DIR, "dos32a.exe")}`
assert_true($?.success? && sver_d32.include?("Version:        9.12"), "Host sver failed on dos32a.exe: #{sver_d32}")
puts "    Staging & native host sver verified (#{host_sver})."

Dir.mktmpdir("dos32a_vagrant_test") do |work|
  puts "==> [2/4] Compiling & linking 32-bit syst32.c via *bwcc386 and *bwlink..."
  bwcc386  = File.join(BINBUILD, "bwcc386")
  bwlink   = File.join(BINBUILD, "bwlink")
  syst_c   = File.join(TEST_DIR, "syst32.c")
  syst_obj = File.join(work, "syst32.obj")
  syst_32  = File.join(work, "syst32.exe")
  syst_bnd = File.join(work, "systbnd.exe")

  ow_env = {
    "OWROOT"    => OW_ROOT,
    "WLINK_LNK" => File.join(OW_ROOT, "build", "bwlink.lnk"),
    "PATH"      => "#{BINBUILD}:#{ENV.fetch('PATH', '')}"
  }

  assert_true(
    system(
      ow_env, bwcc386, "-zq", "-wx", "-we", "-mf", "-bt=dos", "-fpi87", "-s",
      "-I#{File.join(OW_ROOT, 'bld', 'hdr', 'dos', 'h')}",
      "-I#{File.join(OW_ROOT, 'bld', 'watcom', 'h')}",
      "-fo=#{syst_obj}", syst_c
    ),
    "bwcc386 failed on syst32.c"
  )

  assert_true(
    system(ow_env, bwlink, "system", "dos32a", "op", "q", "name", syst_bnd, "file", syst_obj),
    "bwlink system dos32a failed"
  )
  assert_true(
    system(ow_env, bwlink, "system", "stub32a", "op", "q", "name", syst_32, "file", syst_obj),
    "bwlink system stub32a failed"
  )

  bnd_hdr = read_mz_header(syst_bnd)
  assert_true(bnd_hdr[:header_bytes] == 512 && bnd_hdr[:e_lfanew] > 0, "systbnd.exe invalid bound header: #{bnd_hdr.inspect}")
  sver_bnd = `#{host_sver} #{syst_bnd}`
  assert_true($?.success? && sver_bnd.include?("Version:        9.12"), "Host sver failed on systbnd.exe: #{sver_bnd}")

  targets = [
    { vm: "msdos_125", ver: "1.25", drive: "A:", port: 2324, expect_dos: "dos=1.25" },
    { vm: "msdos_20",  ver: "2.0",  drive: "C:", port: 2324, expect_dos: "dos=2.00" },
    { vm: "msdos_40",  ver: "4.0",  drive: "C:", port: 2324, expect_dos: "dos=4.00" }
  ]

  puts "==> [3/4] Running Vagrant + Bento MS-DOS 1.25, 2.00, and 4.00 verification..."
  targets.each do |t|
    vm    = t[:vm]
    ver   = t[:ver]
    drive = t[:drive]
    port  = pick_free_port(t[:port])
    vagrant_env = {
      "VAGRANT_CWD"       => TEST_DIR,
      "MSDOS_SERIAL_PORT" => port.to_s
    }

    puts "  -> Booting Vagrant VM #{vm} (MS-DOS #{ver}, serial port #{port})..."
    begin
      assert_true(
        system(vagrant_env, "vagrant", "up", vm, "--provider=qemu"),
        "vagrant up #{vm} failed"
      )

      comm = open_msdos_communicator(port, ver)
      begin
        puts "     Uploading DOS32A.EXE, SYSTBND.EXE, SYST32.EXE, SVER.EXE to #{drive} via SERGET.COM..."
        comm.fast_upload_file!(File.join(BINW_DIR, "dos32a.exe"), "#{drive}DOS32A.EXE")
        comm.fast_upload_file!(syst_bnd, "#{drive}SYSTBND.EXE")
        comm.fast_upload_file!(syst_32,  "#{drive}SYST32.EXE")
        comm.fast_upload_file!(File.join(BINW_DIR, "sver.exe"), "#{drive}SVER.EXE")

        out_bnd = run_dos_cmd(comm, "SYSTBND --exit0")
        puts "     [#{vm} bound]      #{out_bnd.lines.grep(/SYST32_/).map(&:strip).join(' | ')}"
        assert_true(
          out_bnd.include?("SYST32_PASS") && out_bnd.include?(t[:expect_dos]) && out_bnd.include?("arg1=--exit0"),
          "#{vm}: bound SYSTBND failed: #{out_bnd}"
        )

        # Immediately test standalone DOS32A SYST32.EXE right after bound SYSTBND
        # to verify unified ID32 detection does not match stale SYSTBND in COMMAND.COM memory!
        out_ext = run_dos_cmd(comm, "DOS32A SYST32.EXE --exit0")
        puts "     [#{vm} standalone] #{out_ext.lines.grep(/SYST32_/).map(&:strip).join(' | ')}"
        assert_true(
          out_ext.include?("SYST32_PASS") && out_ext.include?(t[:expect_dos]) && out_ext.include?("arg1=--exit0"),
          "#{vm}: standalone DOS32A SYST32.EXE failed: #{out_ext}"
        )

        out_sver = run_dos_cmd(comm, "SVER DOS32A.EXE")
        assert_true(out_sver.include?("Version:        9.12"), "#{vm}: 16-bit SVER.EXE failed: #{out_sver}")
        puts "     [#{vm} sver.exe]   Verified DOS 16-bit SVER.EXE -> Version 9.12"

        if ver == "4.0"
          out_stub = run_dos_cmd(comm, "SYST32 --exit0")
          assert_true(
            out_stub.include?("SYST32_PASS") && out_stub.include?("dos=4.00"),
            "#{vm}: STUB32A direct execution failed: #{out_stub}"
          )
          puts "     [#{vm} stub32a]    Verified STUB32A.EXE -> DOS32A.EXE chain"

          comm.fast_upload_file!(File.join(BINW_DIR, "sb.exe"), "C:SB.EXE")
          comm.fast_upload_file!(File.join(BINW_DIR, "sc.exe"), "C:SC.EXE")

          out_sb = run_dos_cmd(comm, "SB SYSTBND.EXE")
          assert_true(
            out_sb.include?("DOS/32 Advanced DOS Extender bound to") && out_sb.include?("LE-style"),
            "#{vm}: 32-bit SB.EXE failed: #{out_sb}"
          )
          puts "     [#{vm} sb.exe]     Verified 32-bit SB.EXE inspection"

          out_sc = run_dos_cmd(comm, "SC /B /Q SYST32.EXE")
          assert_true(out_sc.include?("successfully compressed"), "#{vm}: 32-bit SC.EXE failed: #{out_sc}")
          out_lc = run_dos_cmd(comm, "SYST32 --exit0")
          assert_true(
            out_lc.include?("SYST32_PASS") && out_lc.include?("dos=4.00"),
            "#{vm}: LC-compressed SYST32.EXE failed: #{out_lc}"
          )
          puts "     [#{vm} sc.exe]     Verified 32-bit SC.EXE LZSS compression & LC loader"
        end
      ensure
        comm.send(:close_socket) if comm.respond_to?(:close_socket, true)
      end
    ensure
      system(vagrant_env, "vagrant", "destroy", "-f", vm)
    end
  end
end

puts "==> [4/4] ALL VAGRANT MS-DOS 1.25, 2.00, AND 4.00 TESTS PASSED!"
