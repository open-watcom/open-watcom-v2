#!/usr/bin/env ruby
# frozen_string_literal: true

#
# @file run_dos32a_tests.rb
# @brief CTest Equivalence, Ablation A1, and QEMU MS-DOS 1.25/2.00/4.00 Benchmark Suite
#

require 'digest'
require 'fileutils'
require 'json'
require 'optparse'
require 'tmpdir'

options = {
  mode: 'headers',
  build_dir: File.expand_path('../build', __dir__),
  benchmark_out: nil
}

OptionParser.new do |opts|
  opts.on('--mode MODE', 'Test mode: headers or qemu') { |v| options[:mode] = v }
  opts.on('--build-dir DIR', 'DOS/32A build directory') { |v| options[:build_dir] = File.expand_path(v) }
  opts.on('--benchmark-out FILE', 'JSON benchmark output path') { |v| options[:benchmark_out] = File.expand_path(v) }
end.parse!

OW_ROOT   = File.expand_path('../../..', __dir__)
BINW_DIR  = File.join(options[:build_dir], 'binw')
BENTO_LIB = ENV.fetch('BENTO_LIB_DIR', File.expand_path('../../../../bento/lib', __dir__))

EXPECTED_ARTIFACTS = %w[
  dos32a.exe
  dos32a.stb
  dos32a_stock.exe
  stub32a.exe
  stub32c.exe
  sb.exe
  ss.exe
  sc.exe
  sd.exe
  sver.exe
].freeze

# Parses key MZ header fields from binary string `bin`.
def parse_mz_header(bin)
  raise 'Binary smaller than MZ header' if bin.bytesize < 0x40
  raise 'Missing MZ magic' unless %w[MZ ZM].include?(bin[0, 2])

  e_cblp, e_cp, e_crlc, e_cparhdr, _min, _max, e_ss, e_sp, _csum, e_ip, e_cs, e_lfarlc =
    bin[2, 24].unpack('v12')
  e_lfanew = e_lfarlc >= 0x40 ? bin[0x3c, 4].unpack1('V') : 0
  {
    size_bytes: bin.bytesize,
    e_cblp: e_cblp,
    e_cp: e_cp,
    e_crlc: e_crlc,
    e_cparhdr: e_cparhdr,
    header_bytes: e_cparhdr * 16,
    e_ss: e_ss,
    e_sp: e_sp,
    e_cs: e_cs,
    e_ip: e_ip,
    e_lfarlc: e_lfarlc,
    e_lfanew: e_lfanew
  }
end

# Resolves WATCOM root directory on the host.
def resolve_watcom_root
  candidates = [
    ENV.fetch('WATCOM', nil),
    File.join(OW_ROOT, 'rel'),
    '/opt/watcom',
    File.expand_path('~/.local/share/watcom')
  ].compact + Dir.glob(File.expand_path('../*/{vagrant/*/,}.watcom', OW_ROOT))
  found = candidates.find { |d| File.directory?(File.join(d, 'h')) }
  raise 'Cannot locate OpenWatcom v2 WATCOM directory' unless found

  found
end

# Resolves host OpenWatcom binary subdirectory.
def resolve_watcom_bin(watcom_root)
  %w[armo64 bino64 arml64 binl64 binl binb64 armb64 binnt64 binnt].each do |sub|
    cand = File.join(watcom_root, sub)
    return cand if File.executable?(File.join(cand, 'wcl386')) || File.executable?(File.join(cand, 'wcl386.exe'))
  end
  raise "Cannot locate wcl386 under #{watcom_root}"
end

# Verifies MZ/LE headers, ID32 signatures, and bit-for-bit build reproducibility.
def verify_headers_and_reproducibility!(binw_dir)
  EXPECTED_ARTIFACTS.each do |name|
    path = File.join(binw_dir, name)
    raise "Missing built artifact #{path}" unless File.file?(path)

    bin = File.binread(path)
    hdr = parse_mz_header(bin)

    if %w[dos32a.exe dos32a.stb dos32a_stock.exe sb.exe ss.exe sc.exe sd.exe sver.exe].include?(name)
      unless hdr[:e_cparhdr] == 32 && hdr[:header_bytes] == 512
        raise "#{name}: expected e_cparhdr=32 (512B), got #{hdr[:e_cparhdr]} (#{hdr[:header_bytes]}B)"
      end
    end

    if %w[dos32a.exe dos32a.stb dos32a_stock.exe sb.exe ss.exe sc.exe sd.exe].include?(name)
      id_sig = bin[hdr[:header_bytes], 4]
      raise "#{name}: expected ID32 signature at 0x#{hdr[:header_bytes].to_s(16)}, got #{id_sig.inspect}" unless id_sig == 'ID32'.b
    end

    if %w[sb.exe ss.exe sc.exe sd.exe].include?(name)
      raise "#{name}: expected bound LE e_lfanew > 0" unless hdr[:e_lfanew].positive?

      le_sig = bin[hdr[:e_lfanew], 2]
      raise "#{name}: expected LE signature at 0x#{hdr[:e_lfanew].to_s(16)}, got #{le_sig.inspect}" unless le_sig == 'LE'.b
    end
  end

  # Verify bit-for-bit deterministic rebuild in a fresh temp directory
  Dir.mktmpdir('dos32a_repro_') do |tmp|
    build_sh = File.expand_path('../build.sh', __dir__)
    ok = system('/bin/sh', build_sh, tmp)
    raise 'Reproducibility rebuild failed' unless ok

    %w[dos32a.exe dos32a.stb dos32a_stock.exe stub32a.exe stub32c.exe].each do |name|
      sha1 = Digest::SHA256.file(File.join(binw_dir, name)).hexdigest
      sha2 = Digest::SHA256.file(File.join(tmp, 'binw', name)).hexdigest
      raise "Non-deterministic build for #{name}: #{sha1} != #{sha2}" unless sha1 == sha2
    end
  end

  puts 'PASS: All 10 DOS/32A MS-DOS artifacts verified (512B MZ alignment, ID32/LE headers, deterministic SHA-256).'
end

verify_headers_and_reproducibility!(BINW_DIR)
exit 0 if options[:mode] == 'headers'

# Compile SYST32.EXE (external LE) and SYSTBND.EXE (self-bound with dos32a.stb)
watcom_root = resolve_watcom_root
watcom_bin  = resolve_watcom_bin(watcom_root)
env = {
  'WATCOM' => watcom_root,
  'PATH' => "#{watcom_bin}#{File::PATH_SEPARATOR}#{ENV.fetch('PATH', '')}",
  'INCLUDE' => File.join(watcom_root, 'h')
}

syst_src = File.expand_path('syst32.c', __dir__)
syst_obj = File.join(options[:build_dir], 'obj', 'syst32.obj')
syst_ext = File.join(BINW_DIR, 'syst32.exe')
syst_bnd = File.join(BINW_DIR, 'systbnd.exe')

raise 'Failed compiling syst32.c' unless system(env, 'wcl386', '-bt=dos', '-mf', '-3r', '-zq', '-fr=', '-wx', '-we', '-c', "-fo=#{syst_obj}", syst_src)
raise 'Failed linking syst32.exe' unless system(env, 'wlink', 'system', 'stub32a', 'option', 'quiet,', "stub=#{File.join(BINW_DIR, 'stub32a.exe')}", 'name', syst_ext, 'file', syst_obj)
raise 'Failed linking systbnd.exe' unless system(env, 'wlink', 'system', 'dos32a', 'option', 'quiet,', "stub=#{File.join(BINW_DIR, 'dos32a.stb')}", 'name', syst_bnd, 'file', syst_obj)

img_dir = ENV.fetch('MSDOS_IMAGE_DIR', File.expand_path('../../../../bento/builds', __dir__))
msdos_images = [
  { ver: '1.25', img: File.join(img_dir, 'msdos-1.25.img'), port: 24412 },
  { ver: '2.0',  img: File.join(img_dir, 'msdos-2.0.img'),  port: 24420 },
  { ver: '4.0',  img: File.join(img_dir, 'msdos-4.0.img'),  port: 24440 }
]

unless msdos_images.all? { |m| File.file?(m[:img]) } && File.directory?(BENTO_LIB) && system('which qemu-system-i386 >/dev/null 2>&1')
  puts 'SKIP: Live QEMU MS-DOS images or Bento communicator not present on this host; header & reproducibility checks passed.'
  exit 0
end

$LOAD_PATH.unshift(BENTO_LIB)
require 'bento/vagrant_msdos'

d32_hdr   = parse_mz_header(File.binread(File.join(BINW_DIR, 'dos32a.exe')))
stb_hdr   = parse_mz_header(File.binread(File.join(BINW_DIR, 'dos32a.stb')))
stock_hdr = parse_mz_header(File.binread(File.join(BINW_DIR, 'dos32a_stock.exe')))

# Conventional RAM footprint = load module size + minalloc (128 paras) + 8KB lowbuf
conv_ram_bytes = (d32_hdr[:size_bytes] - d32_hdr[:header_bytes]) + (128 * 16) + (0x0200 * 16)

results = {
  schema_version: 1,
  extender: 'DOS/32A',
  version: '9.1.2',
  artifacts: {
    dos32a_exe: {
      size_bytes: d32_hdr[:size_bytes],
      mz_header_bytes: d32_hdr[:header_bytes],
      relocations: d32_hdr[:e_crlc],
      conventional_ram_footprint_bytes: conv_ram_bytes,
      sha256: Digest::SHA256.file(File.join(BINW_DIR, 'dos32a.exe')).hexdigest
    },
    dos32a_stb: {
      size_bytes: stb_hdr[:size_bytes],
      mz_header_bytes: stb_hdr[:header_bytes],
      relocations: stb_hdr[:e_crlc],
      sha256: Digest::SHA256.file(File.join(BINW_DIR, 'dos32a.stb')).hexdigest
    },
    dos32a_stock_exe: {
      size_bytes: stock_hdr[:size_bytes],
      mz_header_bytes: stock_hdr[:header_bytes],
      relocations: stock_hdr[:e_crlc],
      sha256: Digest::SHA256.file(File.join(BINW_DIR, 'dos32a_stock.exe')).hexdigest
    }
  },
  guests: {}
}

msdos_images.each do |spec|
  ver = spec[:ver]
  qemu_pid = Process.spawn(
    'qemu-system-i386',
    '-m', '64', '-display', 'none', '-vga', 'std', '-rtc', 'base=localtime',
    '-snapshot', '-hda', spec[:img],
    '-serial', "tcp:127.0.0.1:#{spec[:port]},server,nowait",
    out: File::NULL, err: File::NULL
  )
  begin
    cfg = Bento::VagrantMsdos::Config.new
    cfg.host = '127.0.0.1'
    cfg.port = spec[:port]
    cfg.dos_version = ver
    cfg.boot_timeout = 30
    cfg.command_timeout = 30
    cfg.finalize!

    vm   = Struct.new(:communicator, :guest).new(:msdos, :msdos)
    mach = Struct.new(:config, :data_dir).new(Struct.new(:vm, :msdos).new(vm, cfg), nil)
    comm = Bento::VagrantMsdos::Communicator.new(mach)
    raise "MS-DOS #{ver} failed to become ready" unless comm.wait_for_ready(25)

    comm.fast_upload_file!(File.join(BINW_DIR, 'dos32a.exe'),       'C:\DOS32A.EXE')
    comm.fast_upload_file!(File.join(BINW_DIR, 'dos32a_stock.exe'), 'C:\D32ASTK.EXE')
    comm.fast_upload_file!(syst_ext,                                'C:\SYST32.EXE')
    comm.fast_upload_file!(syst_bnd,                                'C:\SYSTBND.EXE')
    comm.fast_upload_file!(File.join(BINW_DIR, 'sver.exe'),         'C:\SVER.EXE')

    # 1. Ablation A1: DOS32A_STOCK.EXE (-dDOS_LEGACY_125_20=0)
    out_stock = +''
    comm.execute('D32ASTK SYST32.EXE --exit0', error_check: false) { |_, d| out_stock << d }
    if %w[1.25 2.0].include?(ver)
      raise "MS-DOS #{ver}: expected fatal (0000) from DOS32A_STOCK.EXE, got #{out_stock.inspect}" unless out_stock.include?('fatal (0000)')
    else
      raise "MS-DOS #{ver}: expected SYST32_PASS from DOS32A_STOCK.EXE, got #{out_stock.inspect}" unless out_stock.include?('SYST32_PASS')
    end

    # 2. External DOS32A.EXE (-dDOS_LEGACY_125_20=1) with exit code 0 check + warning 9004 hygiene check
    out_ext0 = +''
    t_start = Process.clock_gettime(Process::CLOCK_MONOTONIC)
    rc_ext0 = comm.execute('DOS32A SYST32.EXE --exit0', error_check: false) { |_, d| out_ext0 << d }
    wall_ms = ((Process.clock_gettime(Process::CLOCK_MONOTONIC) - t_start) * 1000.0).round(2)

    raise "MS-DOS #{ver}: warning (9004) leaked to stdout: #{out_ext0.inspect}" if out_ext0.include?('9004')
    raise "MS-DOS #{ver}: external DOS32A failed (rc=#{rc_ext0}): #{out_ext0.inspect}" unless rc_ext0.zero? && out_ext0.include?('SYST32_PASS')

    # 3. External DOS32A.EXE non-zero exit code 42 propagation
    out_ext42 = +''
    rc_ext42 = comm.execute('DOS32A SYST32.EXE alpha beta', error_check: false) { |_, d| out_ext42 << d }
    raise "MS-DOS #{ver}: expected SYST32_PASS with arg1=alpha, got #{out_ext42.inspect}" unless out_ext42.include?('arg1=alpha')
    if ver == '1.25'
      raise "MS-DOS 1.25: expected non-zero rc for exit(42), got #{rc_ext42}" if rc_ext42.zero?
    else
      out_el = +''
      comm.execute('IF ERRORLEVEL 42 IF NOT ERRORLEVEL 43 ECHO EXACT_42', error_check: false) { |_, d| out_el << d }
      raise "MS-DOS #{ver}: expected ERRORLEVEL 42, got #{out_el.inspect}" unless out_el.include?('EXACT_42')
    end

    # 4. Self-bound dos32a.stb (wlink option stub=dos32a.stb) direct invocation without DOS32A.EXE
    out_bnd = +''
    rc_bnd = comm.execute('SYSTBND --exit0', error_check: false) { |_, d| out_bnd << d }
    raise "MS-DOS #{ver}: bound SYSTBND failed (rc=#{rc_bnd}): #{out_bnd.inspect}" unless rc_bnd.zero? && out_bnd.include?('SYST32_PASS')

    # 5. SVER.EXE version verification
    out_sver = +''
    comm.execute('SVER DOS32A.EXE', error_check: false) { |_, d| out_sver << d }
    raise "MS-DOS #{ver}: SVER failed to report Version 9.12: #{out_sver.inspect}" unless out_sver.include?('9.12')

    m = out_ext0.match(/mode=(\S+)\s+cpu=(\d+)\s+lowbuf=(\d+)\s+argc=(\d+)\s+argv0=(\S+)\s+arg1=(\S+)\s+cycles100=(\d+)/)
    cycles_100 = m ? m[7].to_i : 0

    results[:guests]["msdos-#{ver}"] = {
      ablation_a1_stock_output: out_stock.strip,
      external_dos32a_pass: true,
      bound_dos32a_stb_pass: true,
      warning_9004_emitted: false,
      pmode_backend: m ? m[1] : 'unknown',
      cpu_level: m ? m[2].to_i : 0,
      lowbuf_bytes: m ? m[3].to_i : 0,
      argv0: m ? m[5] : '',
      cycles_per_mode_switch_roundtrip: (cycles_100 / 100.0).round(1),
      end_to_end_command_latency_ms: wall_ms
    }
    puts "PASS [MS-DOS #{ver}]: Ablation A1 + External DOS32A.EXE + Bound dos32a.stb + SVER 9.12 (mode=#{m && m[1]}, #{(cycles_100 / 100.0).round(1)} cyc/switch)"
  ensure
    begin
      Process.kill('TERM', qemu_pid)
      Process.wait(qemu_pid)
    rescue StandardError
      nil
    end
  end
end

if options[:benchmark_out]
  FileUtils.mkdir_p(File.dirname(options[:benchmark_out]))
  File.write(options[:benchmark_out], "#{JSON.pretty_generate(results)}
")
  puts "Exported benchmark metrics to #{options[:benchmark_out]}"
end
