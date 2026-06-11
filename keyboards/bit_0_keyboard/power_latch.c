// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#include "power_latch.h"
#include "quantum.h"
#include "timer.h"

static pin_t    latch_pin    = GP29;
static bool     latch_closed = false;
static bool     button_held  = false;
static uint32_t hold_timer   = 0;

#define POWER_OFF_HOLD_MS 3000

void power_latch_init(uint32_t pin) {
    latch_pin = pin;
    gpio_set_pin_input_high(latch_pin);
    latch_closed = true;
}

void power_latch_close(void) {
    gpio_set_pin_input_high(latch_pin);
    latch_closed = true;
}

void power_latch_open(void) {
    gpio_set_pin_input(latch_pin);
    latch_closed = false;
}

bool power_latch_is_closed(void) {
    return latch_closed;
}

int power_latch_task(void) {
    if (!latch_closed) return 0;

    bool pressed = !gpio_read_pin(latch_pin); // active low (pull-up)

    if (pressed) {
        if (!button_held) {
            hold_timer  = timer_read32();
            button_held = true;
        } else if (timer_elapsed32(hold_timer) >= POWER_OFF_HOLD_MS) {
            power_latch_open();
            return 0;
        }
        return 1;
    } else if (button_held) {
        button_held = false;
    }
    return 0;
}
