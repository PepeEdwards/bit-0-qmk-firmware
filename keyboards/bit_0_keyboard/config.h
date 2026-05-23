// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Bootmagic reset key (hold on plug-in to enter bootloader)
#define BOOTMAGIC_ROW 4
#define BOOTMAGIC_COLUMN 1

// Mouse key settings
#define MOUSEKEY_DELAY          0    // ms before cursor starts moving
#define MOUSEKEY_INTERVAL       16   // ms between cursor steps (~60fps)
#define MOUSEKEY_MOVE_DELTA     3    // pixels per step (initial speed)
#define MOUSEKEY_MAX_SPEED      5    // max speed multiplier
#define MOUSEKEY_TIME_TO_MAX    80   // steps to reach max speed (higher = slower acceleration)

// Led configuration
#define RGBLIGHT_LIMIT_VAL 128 // Limit brightness to 50% to avoid overpowering the LEDs

// Allow keyboard to run without a USB host (UART-only mode)
#define NO_USB_STARTUP_CHECK

// UART configuration (UART0: GP0=TX, GP1=RX)
// UART_TX_PAL_MODE 2 = raw FUNCSEL only — fine for TX output (no IE needed).
// UART_RX_PAL_MODE must NOT be overridden here: chibios_config.h sets it to
// PAL_MODE_ALTERNATE_UART which includes PAL_RP_PAD_IE (bit 30). Without IE,
// the RP2040 PADS_BANK0.IE register bit is 0 and the input buffer is disabled —
// the UART RX FIFO never receives a single byte regardless of what is wired to GP1.
#define UART_DRIVER      SIOD0
#define UART_TX_PIN      GP0
#define UART_TX_PAL_MODE 2
#define UART_RX_PIN      GP1