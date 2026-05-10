// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#include "uart_hid.h"
#include "uart.h"
#include "host.h"
#include "host_driver.h"
#include "report.h"
#include "usb_util.h"

static const host_driver_t *usb_driver;

static void send_frame(uint8_t type, const void *payload, uint8_t len) {
    uart_write(UART_HID_MAGIC);
    uart_write(type);
    uart_write(len);
    uart_transmit((const uint8_t *)payload, len);
}

static uint8_t uart_hid_keyboard_leds(void) {
    return usb_connected_state() ? usb_driver->keyboard_leds() : 0;
}

static void uart_hid_send_keyboard(report_keyboard_t *report) {
    if (usb_connected_state()) usb_driver->send_keyboard(report);
#ifdef KEYBOARD_SHARED_EP
    // Skip the leading report_id byte; bridge expects a bare 8-byte boot report
    send_frame(UART_HID_TYPE_KBD, &report->mods, 8);
#else
    send_frame(UART_HID_TYPE_KBD, report, 8);
#endif
}

static void uart_hid_send_nkro(report_nkro_t *report) {
    if (usb_connected_state()) usb_driver->send_nkro(report);
    send_frame(UART_HID_TYPE_NKRO, report, sizeof(report_nkro_t));
}

static void uart_hid_send_mouse(report_mouse_t *report) {
    if (usb_connected_state()) usb_driver->send_mouse(report);
    send_frame(UART_HID_TYPE_MOUSE, report, sizeof(report_mouse_t));
}

static void uart_hid_send_extra(report_extra_t *report) {
    if (usb_connected_state()) usb_driver->send_extra(report);
    send_frame(UART_HID_TYPE_EXTRA, report, sizeof(report_extra_t));
}

static host_driver_t uart_hid_driver = {
    .keyboard_leds = uart_hid_keyboard_leds,
    .send_keyboard = uart_hid_send_keyboard,
    .send_nkro     = uart_hid_send_nkro,
    .send_mouse    = uart_hid_send_mouse,
    .send_extra    = uart_hid_send_extra,
};

void uart_hid_init(void) {
    uart_init(UART_HID_BAUD);
    usb_driver = host_get_driver();
    host_set_driver(&uart_hid_driver);
}
