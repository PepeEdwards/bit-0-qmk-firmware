// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#include "battery_monitor.h"
#include "i2c_master.h"
#include "timer.h"

#define IP5306_ADDR        (0x75 << 1)
#define REG_CHARGE_STATUS   0x70
#define REG_FULL_STATUS     0x71
#define REG_BATT_LEVEL      0x78
#define CHARGE_BIT          (1 << 3)
#define FULL_BIT            (1 << 3)

#define POLL_INTERVAL_MS    10000
#define I2C_TIMEOUT_MS      10

static battery_status_t current_status = BATT_UNKNOWN;
static uint32_t         last_poll      = 0;
static bool             initialized    = false;

void battery_monitor_init(void) {
    i2c_init();
    initialized = true;
    last_poll   = 0; // sentinel: trigger immediate poll on first task call
}

bool battery_monitor_task(void) {
    if (!initialized) return false;
    if (last_poll != 0 && timer_elapsed32(last_poll) < POLL_INTERVAL_MS) return false;
    last_poll = timer_read32();

    uint8_t charge = 0xFF, full_reg = 0xFF, level = 0xFF;
    if (i2c_read_register(IP5306_ADDR, REG_CHARGE_STATUS, &charge, 1, I2C_TIMEOUT_MS) != I2C_STATUS_SUCCESS) {
        return false;
    }
    i2c_read_register(IP5306_ADDR, REG_FULL_STATUS, &full_reg, 1, I2C_TIMEOUT_MS);
    i2c_read_register(IP5306_ADDR, REG_BATT_LEVEL,  &level,    1, I2C_TIMEOUT_MS);

    battery_status_t new_status;
    if (full_reg & FULL_BIT) {
        new_status = BATT_FULL;
    } else if (charge & CHARGE_BIT) {
        new_status = BATT_CHARGING;
    } else if (level < 0x80) {
        new_status = BATT_HIGH;
    } else if (level < 0xC0) {
        new_status = BATT_MID;
    } else if (level < 0xE0) {
        new_status = BATT_LOW;
    } else {
        new_status = BATT_CRITICAL;
    }

    if (new_status != current_status) {
        current_status = new_status;
        return true;
    }
    return false;
}

battery_status_t battery_get_status(void) {
    return current_status;
}
