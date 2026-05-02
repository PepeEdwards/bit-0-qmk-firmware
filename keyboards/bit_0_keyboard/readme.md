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

## Power Latch
This keyboard has a power latch circuit that allows it to be turned on and off with a physical switch. When the switch is turned on, the keyboard will power up and function normally. When the switch is turned off, the keyboard will power down and stop functioning.
The power latch circuit is controlled by a GPIO pin on the RP2040. When the pin is set high, the latch is closed and the keyboard is powered on. When the pin is set low, the latch is open and the keyboard is powered off.
The power latch circuit is implemented in the `power_latch.c` file, which contains functions
to initialize the latch, open and close the latch, and check the latch status. The latch is initialized in the `keyboard_post_init_user` function, which is called after the keyboard has been initialized. The latch is closed when the keyboard is powered on, and opened when the keyboard is powered off.

# Compile 
To compile the firmware for this keyboard, you can use the following command:

    qmk compile -kb bit_0_keyboard -km default
