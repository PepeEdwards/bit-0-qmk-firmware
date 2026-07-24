# bit_0 — QMK firmware

Firmware for **bit_0_keyboard**, a custom 53-key wireless-capable keyboard built
around an RP2040. This is a self-contained repository: it holds only the custom
keyboard definition and vendors [QMK Firmware](https://github.com/qmk/qmk_firmware)
as a git submodule, so you get a small repo with just the relevant source instead
of a full copy of QMK.

## Features

- **53-key layout** — a 6×7 diode matrix plus 12 direct-wired keys, including 5
  dedicated mouse keys.
- **Three layers** — base QWERTY, an `FN` layer (F-keys, symbols, navigation), and
  a `MEDIA` layer (volume, screen & LED brightness), cycled from a single key.
- **Battery monitoring** — polls an IP5306 power-management IC over I²C and reports
  charge state.
- **Battery/layer-aware RGB** — a single status LED whose colour reflects the active
  layer, shift state, and battery level (with a low-battery pulse).
- **Soft power latch** — hold a side button to physically power the board on/off.
- **UART HID bridge** — mirrors USB HID reports over UART to a companion host.

For a full breakdown of the hardware, pinout, keymap, and each subsystem, see the
keyboard's own docs: [`keyboards/bit_0_keyboard/readme.md`](keyboards/bit_0_keyboard/readme.md).

## Repository structure

```
bit-0-qmk-firmware/
├── build.sh                     one-command build (setup + compile), run from repo root
├── readme.md                    this file
├── .github/workflows/build.yml  CI: compiles firmware on every push, releases on tags
├── keyboards/
│   └── bit_0_keyboard/          ← the actual custom firmware source (edit here)
│       ├── keyboard.json        matrix size, pins, USB IDs, physical layout
│       ├── config.h  rules.mk   build/feature configuration
│       ├── matrix.c             custom matrix + direct-pin scanning
│       ├── battery_monitor.*    IP5306 battery monitor
│       ├── power_latch.*        soft power on/off latch
│       ├── uart_hid.*           UART HID bridge
│       └── keymaps/default/     the default keymap + RGB status logic
└── qmk_firmware/                QMK, vendored as a git submodule (pinned)
```

## How it works — the symlink method

QMK's build system only compiles keyboards that live inside its own
`qmk_firmware/keyboards/` directory. Rather than fork all of QMK to add one
keyboard, this repo keeps the source under its own `keyboards/bit_0_keyboard/`
and creates a **symlink** inside the submodule that points back to it:

```
qmk_firmware/keyboards/bit_0_keyboard  ─►  ../../keyboards/bit_0_keyboard
```

QMK then sees the keyboard as if it were native, while the real files stay in this
repo where you edit and commit them. The symlink is local-only (git can't track a
file that lives inside a submodule), so **it must be re-created after each fresh
clone** — but `build.sh` does that for you automatically, so you never have to.

## Quick start (local build)

**Requirements:** the [QMK CLI](https://docs.qmk.fm/newbs_getting_started) and its
toolchain (`qmk setup`), plus `git` and `bash`.

```sh
git clone <this-repo-url> bit-0-qmk-firmware
cd bit-0-qmk-firmware
./build.sh                 # compile the default keymap
```

`build.sh` is self-healing — on a fresh clone it initialises the `qmk_firmware`
submodule, creates the symlink described above, and compiles. It drives the build
with the `QMK_HOME` environment variable, so your global `qmk config` is never
touched.

```sh
./build.sh <keymap>        # compile a specific keymap
./build.sh default flash    # compile and flash (put the board in bootloader first)
```

The compiled `bit_0_keyboard_default.uf2` is written to the repo root (and is
git-ignored).

### Entering the bootloader (to flash)

- **Bootmagic**: hold the top-left key (Escape) while plugging in the board, or
- **Reset button**: briefly press the reset button on the back of the PCB, or
- **Keycode**: press a key mapped to `QK_BOOT` if your keymap has one.

## Continuous integration (GitHub Actions)

`.github/workflows/build.yml` compiles the firmware in the cloud so you don't have
to build locally to get a flashable file:

- **On every push / pull request** it checks out the repo and the `qmk_firmware`
  submodule, runs `./build.sh` inside the official `ghcr.io/qmk/qmk_cli` container,
  and uploads the resulting `.uf2` as a downloadable **build artifact** (Actions run
  → *Artifacts*).
- **On a pushed tag** it additionally publishes the `.uf2` to a **GitHub Release**:

  ```sh
  git tag v1.0
  git push origin v1.0        # → Release "v1.0" with the firmware attached
  ```

Because CI reuses the same `build.sh` as local builds, there is one source of truth
for how the firmware is compiled.

## Updating QMK

The `qmk_firmware` submodule is pinned to a specific upstream commit for
reproducible builds. To move to a newer QMK:

```sh
cd qmk_firmware
git fetch origin
git checkout <tag-or-commit>          # prefer a release tag over bleeding `master`
git submodule update --init --recursive
cd ..
git add qmk_firmware
git commit -m "chore: bump qmk_firmware to <version>"
./build.sh                            # re-verify it still compiles
```

Your keyboard source is never affected by this — editing `keyboards/bit_0_keyboard/`
and bumping the submodule are fully independent.
