// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "power_latch.h"
#include "uart_hid.h"

enum layers {
    _BASE,
    _FN,
    _MEDIA
};

enum custom_keycodes {
    CYC_LAY = SAFE_RANGE,
    BRT_UP,
    BRT_DN,
};

#define BRT_STEP 25
#define BRT_MIN  10

static uint8_t led_brightness = 200;

static void update_led(layer_state_t state) {
    bool shift_on = (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
    if (shift_on) {
        rgblight_sethsv_noeeprom(43, 255, led_brightness); // orange
    } else if (get_highest_layer(state) == _MEDIA) {
        rgblight_sethsv_noeeprom(231,   255, led_brightness); // red
    } else if (get_highest_layer(state) == _FN) {
        rgblight_sethsv_noeeprom(170, 255, led_brightness); // blue
    } else {
        rgblight_sethsv_noeeprom(85,  255, led_brightness); // green
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CYC_LAY:
            if (record->event.pressed) {
                if (IS_LAYER_ON(_MEDIA)) {
                    layer_off(_MEDIA);
                } else if (IS_LAYER_ON(_FN)) {
                    layer_off(_FN);
                    layer_on(_MEDIA);
                } else {
                    layer_on(_FN);
                }
            }
            return false;
        case BRT_UP:
            if (record->event.pressed) {
                led_brightness = (led_brightness + BRT_STEP > 255) ? 255 : led_brightness + BRT_STEP;
                update_led(layer_state);
            }
            return false;
        case BRT_DN:
            if (record->event.pressed) {
                led_brightness = (led_brightness - BRT_STEP < BRT_MIN) ? BRT_MIN : led_brightness - BRT_STEP;
                update_led(layer_state);
            }
            return false;
    }
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    update_led(state);
    return state;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    update_led(layer_state);
}

void keyboard_post_init_user(void) {
    // GPIO blink test: toggles GP0 for 30s before UART init.
    // Flash this, then run pin_monitor.py on the Lyra during the 30s window.
    gpio_set_pin_output(GP0);
    for (int i = 0; i < 9000; i++) {
        gpio_write_pin_low(GP0);
        wait_ms(2);
        gpio_write_pin_high(GP0);
        wait_ms(2);
    }

    uart_hid_init();
    rgblight_enable_noeeprom();
    update_led(layer_state);
}

void housekeeping_task_user(void) {
    power_latch_task();
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * Base Layer
     * ┌─────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
     * │ ESC │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │
     * ├─────┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
     * │  Q  │ W │ E │ R │ T │ Y │ U │ I │ O │ P │   │
     * ├─────┼───┼───┼───┼───┼───┼───┼───┼───┼───┼─────┤
     * │ TAB │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ENT │
     * ├─────┼───┼───┼───┼───┼───┼───┼───┼───┼───┘─────┘
     * │  Z  │ X │ C │ V │ B │ N │ M │ , │ . │
     * ├─────┼───┼─────┼───────┼─────┼────┼──────┐
     * │ FN  │m_1│LSFT │ SPACE │CTRL |ALT |BKSP  │
     * └─────┴───┴─────┴───────┴─────┴────┴──────┘
     * Mouse: MS_L  MS_D  MS_U  MS_R  MS_CLK
     */
    [_BASE] = LAYOUT_bit0(
        // Row 0: number row
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        // Row 1: top alpha
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        // Row 2: home row
        KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_ENT,
        // Row 3: bottom alpha
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,
        // Row 4: modifiers
        CYC_LAY, MS_BTN1, OSM(MOD_LSFT), KC_SPC,  KC_RCTL, KC_RALT, KC_BSPC,
        // Row 5: mouse
        MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, MS_BTN1
    ),

    /*
     * FN Layer
     * ┌──────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬─────┐
     * │ ESC  │ F1 │ F2 │ F3 │ F4 │ F5 │ F6 │ F7 │ F8 │ F9 │ F10 │
     * ├──────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼─────┤
     * │ F11  │ UP │  | │  _ │  - │  = │  + │  \ │  ' │  " │     │
     * ├──────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼─────┤
     * │      │LEFT│RGHT│  ~ │  ? │  [ │  ] │HOME│  : │  ; │     │
     * ├──────┼────┼────┼────┼────┼────┼────┼────┼────┼────┘─────┘
     * │ F12  │DOWN│  ` │  { │  } │  / │END │  < │  > │
     * └──────┴────┴────┴────┴────┴────┴────┴────┴────┘
     * Mouse: LEFT  DOWN  UP  RIGHT  BTN3
     */
    [_FN] = LAYOUT_bit0(
        // Row 0: F-keys, ESC toggles MEDIA layer
        KC_ESC, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
        // Row 1: Q=F11, E=|, O=', P="
        KC_F11,  KC_UP,   KC_PIPE, KC_UNDS, KC_MINS, KC_EQL,  KC_PLUS, KC_BSLS, KC_QUOT, KC_DQUO,
        // Row 2: D=~, F=?, G=[, H=]
        KC_TAB, KC_LEFT, KC_RGHT, KC_TILD, KC_QUES, KC_LBRC, KC_RBRC, KC_HOME, KC_COLN, KC_SCLN,  KC_ENT,
        // Row 3: Z=F12, C=`, V={, B=}, N=/, M=END
        KC_F12,  KC_DOWN, KC_GRV,  KC_LCBR, KC_RCBR, KC_SLSH, KC_END,  S(KC_COMM), S(KC_DOT),
        // Row 4: modifiers (transparent)
        CYC_LAY, MS_BTN2, OSM(MOD_LSFT), KC_SPC,  KC_RCTL, KC_RALT, KC_BSPC,
        // Row 5: mouse -> arrows, click -> right click
        KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, MS_BTN2
    ),

    /*
     * Media Layer (toggle with FN+ESC)
     * ┌──────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
     * │ ESC  │   │   │   │   │   │   │   │   │   │   │
     * ├──────┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
     * │      │VOL│   │   │   │   │   │   │   │   │   │
     * │      │ ↑ │   │   │   │   │   │   │   │   │   │
     * ├──────┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
     * │      │SCR│SCR│   │   │   │   │   │   │   │   │
     * │      │BRT│BRT│   │   │   │   │   │   │   │   │
     * │      │ ← │ → │   │   │   │   │   │   │   │   │
     * ├──────┼───┼───┼───┼───┼───┼───┼───┼───┼───┘───┘
     * │      │VOL│   │   │   │   │   │LED│LED│
     * │MEDIA │ ↓ │   │   │   │   │   │BRT│BRT│
     * │      │   │   │   │   │   │   │ < │ > │
     * └──────┴───┴───┴───┴───┴───┴───┴───┴───┘
     */
    [_MEDIA] = LAYOUT_bit0(
        // Row 0: toggle off
        KC_ESC, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        // Row 1: W=vol up
        _______, KC_VOLU, _______, _______, _______, _______, _______, _______, _______, _______,
        // Row 2: A=scr brt down, S=scr brt up
        KC_TAB, KC_BRID, KC_BRIU, _______, _______, _______, _______, _______, _______, _______,  KC_ENT,
        // Row 3: X=vol down, ,=led brt down, .=led brt up
        _______, KC_VOLD, _______, _______, _______, _______, _______, BRT_DN,  BRT_UP,
        // Row 4: transparent
        CYC_LAY, MS_BTN2, OSM(MOD_LSFT), KC_SPC,  KC_RCTL, KC_RALT, KC_BSPC,
        // Row 5: transparent
        _______, _______, _______, _______, _______
    )
};
