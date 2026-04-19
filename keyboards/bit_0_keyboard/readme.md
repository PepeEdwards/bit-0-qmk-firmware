# bit_0_keyboard

Custom 53-key keyboard with RP2040, 6x7 matrix plus 12 direct-pin keys including mouse controls.

* Keyboard Maintainer: [PepeEdwards](https://github.com/PepeEdwards)
* Hardware Supported: Bit-0 custom PCB with RP2040
* Hardware Availability: Custom build

Make example for this keyboard (after setting up your build environment):

    make bit_0_keyboard:default

Flashing example for this keyboard:

    make bit_0_keyboard:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
