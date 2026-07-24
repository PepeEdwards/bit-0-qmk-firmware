#!/usr/bin/env bash
# Build bit_0_keyboard from the userspace repo root.
#
# Usage:
#   ./build.sh                 # compile the default keymap
#   ./build.sh <keymap>        # compile a specific keymap
#   ./build.sh <keymap> flash  # compile and flash
#
# Self-healing: initialises the qmk_firmware submodule and (re)creates the
# symlink that exposes keyboards/bit_0_keyboard to QMK, so a fresh clone just
# works with no manual setup. Never writes global `qmk config` — it drives the
# build with the QMK_HOME environment variable instead.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_DIR="$REPO_ROOT/qmk_firmware"
KEYBOARD="bit_0_keyboard"
KEYMAP="${1:-default}"
TARGET="${2:-}"   # pass "flash" as the 2nd arg to flash

# 1. Ensure the qmk_firmware submodule (and its nested deps) are present.
if [ ! -f "$QMK_DIR/Makefile" ]; then
  echo "==> qmk_firmware submodule missing; initialising..."
  git -C "$REPO_ROOT" submodule update --init --recursive
fi

# 2. Ensure the symlink that makes QMK see our keyboard exists (idempotent).
LINK="$QMK_DIR/keyboards/$KEYBOARD"
if [ ! -L "$LINK" ]; then
  echo "==> creating keyboard symlink into qmk_firmware..."
  ln -s "../../keyboards/$KEYBOARD" "$LINK"
fi

# 3. Build (or flash). QMK resolves its firmware root from the current working
#    directory (QMK_FIRMWARE = Path.cwd()), NOT from a global config or the
#    QMK_HOME env var, so we must cd into the submodule. This makes the build
#    independent of whatever `qmk config user.qmk_home` happens to point at.
export QMK_HOME="$QMK_DIR"   # belt-and-suspenders; cwd is the real determinant
cd "$QMK_DIR"
if [ "$TARGET" = "flash" ]; then
  exec qmk flash -kb "$KEYBOARD" -km "$KEYMAP"
fi

qmk compile -kb "$KEYBOARD" -km "$KEYMAP"

# Copy the built firmware to the repo root so the output location is
# deterministic. QMK's own "copy to qmk_firmware folder" step targets the
# globally-configured qmk_home, which may be an unrelated checkout — we don't
# rely on it.
found=0
for ext in uf2 bin hex; do
  fw="$QMK_DIR/.build/${KEYBOARD}_${KEYMAP}.$ext"
  if [ -f "$fw" ]; then
    cp "$fw" "$REPO_ROOT/"
    echo "==> firmware: $REPO_ROOT/$(basename "$fw")"
    found=1
  fi
done
[ "$found" -eq 1 ] || { echo "error: no firmware produced in $QMK_DIR/.build" >&2; exit 1; }
