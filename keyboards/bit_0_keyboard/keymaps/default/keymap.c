// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    _BASE,
    _FN
};

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
     * │ FN  │ALT│LSFT │ SPACE │RSFT │CTRL│ BKSP │
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
        MO(_FN), KC_LALT, KC_LSFT, KC_SPC,  KC_RSFT, KC_LCTL, KC_BSPC,
        // Row 5: mouse
        MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, MS_BTN1
    ),

    /*
     * FN Layer
     * ┌─────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬─────┐
     * │     │ F1 │ F2 │ F3 │ F4 │ F5 │ F6 │ F7 │ F8 │ F9 │ F10 │
     * ├─────┼────┼────┼────┼────┼────┼────┼────┼────┼─────┼─────┤
     * │  ~  │ UP │  ` │  _ │  - │  = │  + │  \ │F11 │ F12 │     │
     * ├─────┼────┼────┼────┼────┼────┼────┼────┼────┼─────┼─────┤
     * │     │LEFT│RGHT│  : │  " │  { │  } │    │    │HOME │     │
     * ├─────┼────┼────┼────┼────┼────┼────┼────┼─────┘─────┘
     * │  |  │DOWN│  ; │  ' │  [ │  ] │  ? │  / │ END │
     * └─────┴────┴────┴────┴────┴────┴────┴────┴─────┘
     * Mouse: LEFT  DOWN  UP  RIGHT  BTN2
     */
    [_FN] = LAYOUT_bit0(
        // Row 0: F-keys
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
        // Row 1: symbols
        KC_TILD, KC_UP,   KC_GRV,  KC_UNDS, KC_MINS, KC_EQL,  KC_PLUS, KC_BSLS, KC_F11,  KC_F12,
        // Row 2: nav + brackets
        _______, KC_LEFT, KC_RGHT, KC_COLN, KC_DQUO, KC_LCBR, KC_RCBR, XXXXXXX, XXXXXXX, KC_HOME, _______,
        // Row 3: more symbols
        KC_PIPE, KC_DOWN, KC_SCLN, KC_QUOT, KC_LBRC, KC_RBRC, KC_QUES, KC_SLSH, KC_END,
        // Row 4: modifiers (transparent)
        _______, _______, _______, _______, _______, _______, _______,
        // Row 5: mouse -> arrow keys, click -> right click
        KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, MS_BTN2
    )
};
