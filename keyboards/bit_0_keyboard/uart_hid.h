// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>

// UART baud rate for HID report forwarding
#define UART_HID_BAUD 115200

// Frame: [0xAA][TYPE][LEN][payload...]
#define UART_HID_MAGIC      0xAA
#define UART_HID_TYPE_KBD   0x01  // RP2040→Lyra: 8-byte boot keyboard report
#define UART_HID_TYPE_NKRO  0x02  // RP2040→Lyra: NKRO bitfield
#define UART_HID_TYPE_MOUSE 0x03  // RP2040→Lyra: mouse report
#define UART_HID_TYPE_EXTRA 0x04  // RP2040→Lyra: consumer/system report
#define UART_HID_TYPE_READY 0xFF  // Lyra→RP2040: bridge is up, start sending

// Installs the UART mirror on top of the existing USB host driver.
// HID reports are held until uart_hid_task() receives the Lyra READY beacon.
// Call once from keyboard_post_init_user().
void uart_hid_init(void);

// Returns true once the Lyra READY beacon has been received.
bool uart_hid_is_ready(void);

// Polls the UART RX buffer for incoming bytes and parses frames.
// Returns:
//   0 — nothing received this call
//   1 — received bytes but READY beacon not yet seen (LED diagnostic)
//   2 — READY beacon just received (or 90 s fallback triggered)
// Call from housekeeping_task_user().
int uart_hid_task(void);
