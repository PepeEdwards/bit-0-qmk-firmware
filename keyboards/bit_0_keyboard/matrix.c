// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#include "matrix.h"
#include "gpio.h"
#include "wait.h"
#include "power_latch.h"

// 6x7 physical matrix: COL2ROW (columns are read, rows are driven)
static const pin_t col_pins[7] = {GP12, GP13, GP14, GP15, GP16, GP17, GP18};
static const pin_t row_pins[6] = {GP7, GP8, GP9, GP10, GP11, GP2};

// Row 6: FN1(GP19) FN2(GP20) FN3(GP21) FN4(GP22) FN5(GP3) FN6(GP4) FN8(GP5)
// Row 7: FN9(GP6) FN10(GP23) FN11(GP24) FN12(GP25)
static const pin_t direct_row6[7] = {GP19, GP20, GP21, GP22, GP3, GP4, GP5};
static const pin_t direct_row7[4] = {GP6, GP23, GP24, GP25};

void matrix_init_custom(void) {
    power_latch_init(GP29);

    for (int c = 0; c < 7; c++) {
        gpio_set_pin_input_high(col_pins[c]);
    }

    for (int r = 0; r < 6; r++) {
        gpio_set_pin_output(row_pins[r]);
        gpio_write_pin_high(row_pins[r]);
    }

    for (int i = 0; i < 7; i++) {
        gpio_set_pin_input_high(direct_row6[i]);
    }
    for (int i = 0; i < 4; i++) {
        gpio_set_pin_input_high(direct_row7[i]);
    }
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    for (int r = 0; r < 6; r++) {
        gpio_write_pin_low(row_pins[r]);
        wait_us(5);

        matrix_row_t row_data = 0;
        for (int c = 0; c < 7; c++) {
            if (!gpio_read_pin(col_pins[c])) {
                row_data |= ((matrix_row_t)1 << c);
            }
        }

        gpio_write_pin_high(row_pins[r]);

        if (current_matrix[r] != row_data) {
            current_matrix[r] = row_data;
            changed = true;
        }
    }

    // Scan virtual row 6 (direct pins)
    {
        matrix_row_t row_data = 0;
        for (int i = 0; i < 7; i++) {
            if (!gpio_read_pin(direct_row6[i])) {
                row_data |= ((matrix_row_t)1 << i);
            }
        }
        if (current_matrix[6] != row_data) {
            current_matrix[6] = row_data;
            changed = true;
        }
    }

    // Scan virtual row 7 (direct pins)
    {
        matrix_row_t row_data = 0;
        for (int i = 0; i < 4; i++) {
            if (!gpio_read_pin(direct_row7[i])) {
                row_data |= ((matrix_row_t)1 << i);
            }
        }
        if (current_matrix[7] != row_data) {
            current_matrix[7] = row_data;
            changed = true;
        }
    }

    return changed;
}
