#!/bin/sh
#
# @file build.sh
# @brief Hermetic cross-platform POSIX build script for MS-DOS builds of DOS/32A v9.1.2
# @description
#   Assembles and links standalone DOS32A.EXE (-dDOS_LEGACY_125_20=1),
#   bindable dos32a.stb (-dDOS_LEGACY_125_20=1 -dDOS32A_BOUND_STUB=1),
#   ablation stock DOS32A_STOCK.EXE (-dDOS_LEGACY_125_20=0), STUB32A.EXE,
#   STUB32C.EXE, SB.EXE, SS.EXE, SC.EXE, SD.EXE, and SVER.EXE using Open Watcom v2
#   (wasm, wcl, wcl386, wlink) with zero dependency on proprietary TASM.EXE.
#

set -eu

SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
OW_ROOT=$(CDPATH='' cd -- "${SCRIPT_DIR}/../.." && pwd)
D32_ROOT="${OW_ROOT}/contrib/extender/dos32a"
D32_SRC="${D32_ROOT}/src"
OUT_DIR="${1:-${SCRIPT_DIR}/build}"

resolve_watcom() {
  if [ -n "${WATCOM:-}" ] && [ -d "${WATCOM}/h" ]; then
    printf '%s' "${WATCOM}"
    return 0
  fi
  for cand in "${OW_ROOT}/rel" "/opt/watcom" "${HOME}/.local/share/watcom" "${HOME}/watcom"; do
    if [ -d "${cand}/h" ]; then
      printf '%s' "$(CDPATH='' cd -- "${cand}" && pwd)"
      return 0
    fi
  done
  for cand in "${OW_ROOT}"/../*/vagrant/*/.watcom "${OW_ROOT}"/../*/.watcom; do
    if [ -d "${cand}/h" ]; then
      printf '%s' "$(CDPATH='' cd -- "${cand}" && pwd)"
      return 0
    fi
  done
  echo "ERROR: WATCOM environment variable not set and no OpenWatcom v2 installation found." >&2
  return 1
}

resolve_host_bin() {
  _w="$1"
  _os=$(uname -s)
  _arch=$(uname -m)
  case "${_os}:${_arch}" in
    Darwin:arm64|Darwin:aarch64) _sub="armo64" ;;
    Darwin:x86_64)               _sub="bino64" ;;
    Linux:aarch64|Linux:arm64)   _sub="arml64" ;;
    Linux:x86_64)                _sub="binl64" ;;
    Linux:*)                     _sub="binl"   ;;
    FreeBSD:aarch64|FreeBSD:arm64) _sub="armb64" ;;
    FreeBSD:*)                   _sub="binb64" ;;
    *)                           _sub="binl64" ;;
  esac
  if [ -x "${_w}/${_sub}/wasm" ]; then
    printf '%s' "${_w}/${_sub}"
    return 0
  fi
  for fb in armo64 bino64 arml64 binl64 binl binb64 armb64 binnt64 binnt; do
    if [ -x "${_w}/${fb}/wasm" ]; then
      printf '%s' "${_w}/${fb}"
      return 0
    fi
  done
  echo "ERROR: Cannot locate OpenWatcom host binaries under ${_w}." >&2
  return 1
}

WATCOM=$(resolve_watcom)
OW_BIN=$(resolve_host_bin "${WATCOM}")
export WATCOM
export PATH="${OW_BIN}:${PATH}"
export INCLUDE="${WATCOM}/h:${D32_ROOT}/h32:${D32_SRC}/sutils/misc"

mkdir -p "${OUT_DIR}"
OUT_DIR=$(CDPATH='' cd -- "${OUT_DIR}" && pwd)
OBJ_DIR="${OUT_DIR}/obj"
BIN_DIR="${OUT_DIR}/binw"
mkdir -p "${OBJ_DIR}" "${BIN_DIR}"

CC_HOST="${CC:-cc}"
"${CC_HOST}" -std=c89 -O2 -Wall -Wextra -Werror -pedantic "${D32_SRC}/sutils/mzalign/mzalign.c" -o "${OBJ_DIR}/mzalign"

WASM_COMMON="-bt=dos -fpi87 -wx -we -zld -zq -dEXEC_TYPE=0"

# 1. DOS32A.EXE (Pre-DOS-4.0 compatible: MS-DOS 1.25, 2.00, 3.x, 4.00+)
(
  cd "${D32_SRC}/dos32a"
  wasm ${WASM_COMMON} -dDOS_LEGACY_125_20=1 -dDOS32A_BOUND_STUB=0 -i="../sutils/misc" -i="." -i="text" -fo="${OBJ_DIR}/kernel.obj" kernel.asm
  wasm ${WASM_COMMON} -dDOS_LEGACY_125_20=1 -dDOS32A_BOUND_STUB=0 -i="../sutils/misc" -i="." -i="text" -fo="${OBJ_DIR}/dos32a.obj" dos32a.asm
)
wlink system dos option quiet, fullheader, packcode=0, packdata=0, map="${OBJ_DIR}/dos32a.map" name "${BIN_DIR}/dos32a.exe" file "${OBJ_DIR}/dos32a.obj", "${OBJ_DIR}/kernel.obj"
"${OBJ_DIR}/mzalign" "${BIN_DIR}/dos32a.exe" "${BIN_DIR}/dos32a.exe" --pad-relocs 0x007E

# 2. dos32a.stb (Bindable stub for wlink option stub=dos32a.stb across MS-DOS 1.25-4.00+)
(
  cd "${D32_SRC}/dos32a"
  wasm ${WASM_COMMON} -dDOS_LEGACY_125_20=1 -dDOS32A_BOUND_STUB=1 -i="../sutils/misc" -i="." -i="text" -fo="${OBJ_DIR}/dos32a_stb.obj" dos32a.asm
)
wlink system dos option quiet, fullheader, packcode=0, packdata=0, map="${OBJ_DIR}/dos32a_stb.map" name "${BIN_DIR}/dos32a.stb" file "${OBJ_DIR}/dos32a_stb.obj", "${OBJ_DIR}/kernel.obj"
"${OBJ_DIR}/mzalign" "${BIN_DIR}/dos32a.stb" "${BIN_DIR}/dos32a.stb" --pad-relocs 0x007E

# 3. DOS32A_STOCK.EXE (Stock DOS >= 4.0 gate for Ablation A1)
(
  cd "${D32_SRC}/dos32a"
  wasm ${WASM_COMMON} -dDOS_LEGACY_125_20=0 -dDOS32A_BOUND_STUB=0 -i="../sutils/misc" -i="." -i="text" -fo="${OBJ_DIR}/kernel_stock.obj" kernel.asm
  wasm ${WASM_COMMON} -dDOS_LEGACY_125_20=0 -dDOS32A_BOUND_STUB=0 -i="../sutils/misc" -i="." -i="text" -fo="${OBJ_DIR}/dos32a_stock.obj" dos32a.asm
)
wlink system dos option quiet, fullheader, packcode=0, packdata=0, map="${OBJ_DIR}/dos32a_stock.map" name "${BIN_DIR}/dos32a_stock.exe" file "${OBJ_DIR}/dos32a_stock.obj", "${OBJ_DIR}/kernel_stock.obj"
"${OBJ_DIR}/mzalign" "${BIN_DIR}/dos32a_stock.exe" "${BIN_DIR}/dos32a_stock.exe" --pad-relocs 0x007E

# 4. STUB32A.EXE & STUB32C.EXE
(
  cd "${D32_SRC}/stub32a"
  wasm -bt=dos -wx -we -zld -zq -dEXEC_TYPE=0 -i="../sutils/misc" -fo="${OBJ_DIR}/stub32a.obj" stub32a.asm
  wasm -bt=dos -wx -we -zld -zq -dEXEC_TYPE=0 -i="../sutils/misc" -fo="${OBJ_DIR}/stub32c.obj" stub32c.asm
)
wlink system dos option quiet, fullheader name "${BIN_DIR}/stub32a.exe" file "${OBJ_DIR}/stub32a.obj"
wlink system dos option quiet, fullheader name "${BIN_DIR}/stub32c.exe" file "${OBJ_DIR}/stub32c.obj"

# 5. Companion MS-DOS Tools: SB.EXE, SS.EXE, SC.EXE, SD.EXE, SVER.EXE
WCL32_FLAGS="-zq -fr= -oneatx -ohirbk -ol+ -ei -zp16 -6 -fp6 -fpi87 -bt=dos"

(
  cd "${D32_SRC}/sb"
  wasm -bt=dos -3p -mf -wx -we -zld -zq -i="../sutils/misc" -fo="${OBJ_DIR}/sbind.obj" sbind.asm
  wcl386 ${WCL32_FLAGS} -c -fo="${OBJ_DIR}/sb_main.obj" main.c
)
wlink system dos32a option quiet, stub="${BIN_DIR}/dos32a.stb", stack=65536 name "${BIN_DIR}/sb.exe" file "${OBJ_DIR}/sbind.obj", "${OBJ_DIR}/sb_main.obj"

(
  cd "${D32_SRC}/ss"
  wasm -bt=dos -3p -mf -wx -we -zld -zq -i="../sutils/misc" -fo="${OBJ_DIR}/setup.obj" setup.asm
  wcl386 ${WCL32_FLAGS} -c -fo="${OBJ_DIR}/ss_main.obj" main.c
)
wlink system dos32a option quiet, stub="${BIN_DIR}/dos32a.stb", stack=65536 name "${BIN_DIR}/ss.exe" file "${OBJ_DIR}/setup.obj", "${OBJ_DIR}/ss_main.obj"

(
  cd "${D32_SRC}/sc"
  wasm -bt=dos -3p -mf -wx -we -zld -zq -i="../sutils/misc" -fo="${OBJ_DIR}/scomp.obj" scomp.asm
  wasm -bt=dos -3p -mf -wx -we -zld -zq -i="../sutils/misc" -fo="${OBJ_DIR}/sload.obj" sload.asm
  wcl386 ${WCL32_FLAGS} -c -fo="${OBJ_DIR}/sc_encode.obj" encode.c
  wcl386 ${WCL32_FLAGS} -c -fo="${OBJ_DIR}/sc_main.obj" main.c
)
wlink system dos32a option quiet, stub="${BIN_DIR}/dos32a.stb", stack=65536 name "${BIN_DIR}/sc.exe" file "${OBJ_DIR}/scomp.obj", "${OBJ_DIR}/sload.obj", "${OBJ_DIR}/sc_encode.obj", "${OBJ_DIR}/sc_main.obj"

(
  cd "${D32_SRC}/sd"
  wasm -bt=dos -4p -mf -fpi87 -wx -we -zld -zq -dSVR=0 -i="../sutils/misc" -fo="${OBJ_DIR}/sd.obj" sd.asm
)
wlink system dos32a option quiet, stub="${BIN_DIR}/dos32a.stb" name "${BIN_DIR}/sd.exe" file "${OBJ_DIR}/sd.obj"

(
  cd "${D32_SRC}/sutils/sver"
  wcl -zq -fr= -oneatx -bt=dos -ml -c -fo="${OBJ_DIR}/sver.obj" main.c
)
wlink system dos option quiet, fullheader name "${BIN_DIR}/sver.exe" file "${OBJ_DIR}/sver.obj"
"${OBJ_DIR}/mzalign" "${BIN_DIR}/sver.exe" "${BIN_DIR}/sver.exe"

echo "==> [DOS/32A] Built MS-DOS artifacts in ${BIN_DIR}"
