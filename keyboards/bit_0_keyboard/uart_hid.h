// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// UART baud rate for HID report forwarding
#define UART_HID_BAUD 115200

// Frame: [0xAA][TYPE][LEN][raw HID report bytes...]
// The SBC writes LEN bytes starting after the header directly to /dev/hidg*
#define UART_HID_MAGIC      0xAA
#define UART_HID_TYPE_KBD   0x01  // standard 8-byte keyboard report
#define UART_HID_TYPE_NKRO  0x02  // NKRO bitfield report
#define UART_HID_TYPE_MOUSE 0x03  // mouse report
#define UART_HID_TYPE_EXTRA 0x04  // consumer/system report

// Installs the UART mirror on top of the existing USB host driver.
// All HID reports continue to go to USB AND are sent over UART.
// Call once from keyboard_post_init_user().
void uart_hid_init(void);
