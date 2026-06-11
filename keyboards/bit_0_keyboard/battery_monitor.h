// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BATT_UNKNOWN = 0,
    BATT_CRITICAL,  // < 25%  (reg >= 0xE0)
    BATT_LOW,       // 25-49% (reg 0xC0-0xDF)
    BATT_MID,       // 50-74% (reg 0x80-0xBF)
    BATT_HIGH,      // 75-100%(reg 0x00-0x7F)
    BATT_CHARGING,
    BATT_FULL,
} battery_status_t;

void             battery_monitor_init(void);
bool             battery_monitor_task(void); // true when status changed
battery_status_t battery_get_status(void);
