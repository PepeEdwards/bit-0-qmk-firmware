# bit_0_keyboard

Custom 53-key keyboard built around an RP2040: a 6x7 diode matrix plus 12 direct-pin keys (including 5 dedicated mouse keys), UART HID bridging to a second host ("Lyra"), an IP5306-based battery monitor, and battery/layer-aware RGB status feedback.

* Keyboard Maintainer: [PepeEdwards](https://github.com/PepeEdwards)
* Hardware Supported: Bit-0 custom PCB with RP2040
* Hardware Availability: Custom build

## File tree

```
bit_0_keyboard/
├── keyboard.json              Keyboard metadata: matrix size, pins, USB IDs, LAYOUT_bit0 physical layout
├── config.h                   Bootmagic key, mouse key tuning, RGB limit, UART0 pins, I2C1 pins
├── rules.mk                   Build config: custom matrix, UART/I2C drivers, extra source files
├── matrix.c                   Custom matrix scanning (6x7 matrix + 2 rows of direct-pin keys)
├── power_latch.c / .h         Soft power switch: latch GPIO, hold-to-power-off logic
├── battery_monitor.c / .h     I2C polling of the IP5306 fuel gauge/charger IC
├── uart_hid.c / .h            UART bridge that mirrors USB HID reports to a second host
├── keyboard_layout_reference.json  Reference copy of the physical key layout
├── test.txt                   (scratch/unused)
└── keymaps/
    └── default/
        └── keymap.c            Base/FN/Media layers, RGB status logic, custom keycodes
```

## Features

### 53-key layout with mouse keys (`keyboard.json`, `matrix.c`)
- 6x7 diode matrix (42 keys) scanned in the standard way, `COL2ROW`.
- 2 extra "virtual" rows (11 pins) wired directly to GPIO with no diodes, used for a Fn cluster and 5 dedicated mouse keys (left/down/up/right/click).
- Defined as `LAYOUT_bit0` in `keyboard.json`, with matrix coordinates and physical `x`/`y` positions for each key.

### Three-layer keymap (`keymaps/default/keymap.c`)
- **`_BASE`** — standard QWERTY plus one-shot shift and mouse keys.
- **`_FN`** — F-keys, arrows, symbols, navigation (Home/End), and mouse keys remapped to arrows/right-click.
- **`_MEDIA`** — volume, screen brightness, and LED brightness controls.
- Custom keycode `CYC_LAY` cycles `_BASE → _FN → _MEDIA → _BASE` from a single key.
- Custom keycodes `BRT_UP` / `BRT_DN` adjust the RGB LED brightness in-place (not layer changes).

### Battery-aware single RGB LED (`keymap.c`, `keyboard.json`)
- One WS2812 LED on `GP28`, brightness-limited to 50% (`RGBLIGHT_LIMIT_VAL`) to avoid overdriving it.
- Color reflects state, in priority order: Shift held (cyan) → `_MEDIA` layer (purple) → `_FN` layer (blue) → battery status (green/orange/red) on the base layer.
- Battery-critical state pulses/breathes red while idle on the base layer.
- Boots white until the UART bridge to the second host comes up, then flashes green 3x once the `READY` beacon is received.

### Battery monitor (`battery_monitor.c/.h`)
- Polls an IP5306 power-management IC over I2C1 (`GP26`/`GP27`, address `0x75`) every 10 seconds.
- Reads charge, full-charge, and battery-level registers to classify status: `CRITICAL` (<25%), `LOW` (25–49%), `MID` (50–74%), `HIGH` (75–100%), `CHARGING`, or `FULL`.
- Reports back only when the status actually changes, so the keymap can update the LED without polling every scan.

### Power latch (`power_latch.c/.h`)
- Lets the board be turned on/off with a physical switch/button wired to `GP29`, instead of requiring a USB unplug.
- Holding the button for 3 seconds (`POWER_OFF_HOLD_MS`) opens the latch and powers the board down; the LED blinks red while held.
- The latch is closed (powered on) automatically at boot, in both `matrix_init_custom()` and `keyboard_post_init_user()`.

### UART HID bridge (`uart_hid.c/.h`)
- Installs a second `host_driver_t` that mirrors every USB HID report (keyboard, NKRO, mouse, extra/consumer) out over UART0 (`GP0` TX / `GP1` RX at 115200 baud) to a companion device referred to as "Lyra".
- Simple framed protocol: `[0xAA][TYPE][LEN][payload]`, with a `READY` (`0xFF`) beacon the peer sends once it's listening.
- Reports are buffered/held until the `READY` beacon arrives (or a 90-second fallback timeout elapses), so nothing is lost if Lyra boots slower than the keyboard.
- Sends a heartbeat frame every 2 seconds while waiting, so the link direction can be verified independently of the beacon.
- Installed from `housekeeping_task_user()` rather than `keyboard_post_init_user()`, since QMK's `protocol_post_init()` would otherwise overwrite it with the default USB driver.

### Other configuration
- `NKRO` is disabled and `bootmagic`/`extrakey`/`mousekey`/`rgblight` are enabled (`keyboard.json`).
- Bootmagic reset key is matrix position `(4,1)`.
- `NO_USB_STARTUP_CHECK` lets the board run standalone over UART without a USB host attached.

## Getting started

This board is a standard QMK keyboard directory, so the normal QMK workflow applies. If you're new to QMK, start with the [Complete Newbs Guide](https://docs.qmk.fm/newbs_getting_started) — the summary below is specific to this keyboard.

1. **Set up your build environment** (once per machine): follow [QMK's environment setup guide](https://docs.qmk.fm/newbs_getting_started#set-up-your-environment) to install the `qmk` CLI and toolchain, then run:
   ```
   qmk setup
   ```
2. **Build the firmware**:
   ```
   qmk compile -kb bit_0_keyboard -km default
   ```
   or, from a `qmk_firmware` checkout:
   ```
   make bit_0_keyboard:default
   ```
3. **Flash the firmware**, after putting the board in bootloader mode (see below):
   ```
   qmk flash -kb bit_0_keyboard -km default
   ```
   or:
   ```
   make bit_0_keyboard:default:flash
   ```
4. **Customize the keymap**: copy `keymaps/default` to a new folder (e.g. `keymaps/mine`) and edit `keymap.c`, then build/flash with `-km mine` instead of `-km default`. See [QMK's keymap guide](https://docs.qmk.fm/newbs_learn_more_resources) for keycode references.

### Entering the bootloader

Use any of the following to flash new firmware:
* **Bootmagic reset**: hold the key at matrix position (0,0) (top-left / Escape) while plugging in the keyboard.
* **Physical reset button**: briefly press the reset button on the back of the PCB (some revisions expose pads to short instead).
* **Keycode in layout**: press a key mapped to `QK_BOOT`, if one is present in your keymap.

See QMK's [build environment setup](https://docs.qmk.fm/newbs_getting_started#set-up-your-environment) and [make/compile guide](https://docs.qmk.fm/newbs_building_firmware) for more detail on any of the steps above.
