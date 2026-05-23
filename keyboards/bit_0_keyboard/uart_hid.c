// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#include "uart_hid.h"
#include "uart.h"
#include "host.h"
#include "host_driver.h"
#include "report.h"
#include "usb_util.h"
#include "timer.h"
#include <hal.h>  // chnGetTimeout / TIME_IMMEDIATE / STM_TIMEOUT (ChibiOS)

static const host_driver_t *usb_driver;

// HID frames are held until the Lyra bridge sends UART_HID_TYPE_READY.
static bool lyra_ready = false;

// ── RX parser ────────────────────────────────────────────────────────────────
// Minimal 3-state machine: wait for magic → read type → skip payload bytes.
// Only acts on UART_HID_TYPE_READY; all other incoming frame types are ignored.

typedef enum { RX_MAGIC = 0, RX_TYPE, RX_LEN, RX_PAYLOAD } rx_state_t;

static rx_state_t rx_state   = RX_MAGIC;
static uint8_t    rx_type    = 0;
static uint8_t    rx_remain  = 0;
static uint32_t   init_time  = 0;

#define LYRA_READY_TIMEOUT_MS 90000  // auto-enable after 90 s if no beacon

bool uart_hid_is_ready(void) { return lyra_ready; }

int uart_hid_task(void) {
    int status = 0;

    // sioSynchronizeRX (called internally by chnGetTimeout / uart_read) checks
    // error flags BEFORE checking the FIFO and loops forever returning
    // SIO_MSG_ERRORS if any framing/overrun bits are set — which happens
    // routinely from line noise on a floating RX pin at boot.
    //
    // Fix: clear those stale error flags here so they don't block anything,
    // then read with uart_available() + sioGetX():
    //   uart_available() → sioIsRXEmptyX → reads UARTFR.RXFE hardware register
    //   sioGetX()        → sio_lld_get   → reads UARTDR hardware register
    // Both are direct register reads that bypass the OS event/error system.
    if (sioHasRXErrorsX(&UART_DRIVER)) {
        sioGetAndClearErrors(&UART_DRIVER);
    }

    while (uart_available()) {
        uint8_t b = (uint8_t)sioGetX(&UART_DRIVER);
        status = 1;

        switch (rx_state) {
            case RX_MAGIC:
                if (b == UART_HID_MAGIC) rx_state = RX_TYPE;
                break;
            case RX_TYPE:
                rx_type  = b;
                rx_state = RX_LEN;
                break;
            case RX_LEN:
                rx_remain = b;
                if (rx_remain == 0) {
                    if (rx_type == UART_HID_TYPE_READY && !lyra_ready) {
                        lyra_ready = true;
                        status     = 2;
                    }
                    rx_state = RX_MAGIC;
                } else {
                    rx_state = RX_PAYLOAD;
                }
                break;
            case RX_PAYLOAD:
                if (--rx_remain == 0) {
                    if (rx_type == UART_HID_TYPE_READY && !lyra_ready) {
                        lyra_ready = true;
                        status     = 2;
                    }
                    rx_state = RX_MAGIC;
                }
                break;
        }
    }

    // TX heartbeat while waiting: send 0xAA 0xFE 0x00 every 2 s so the Lyra
    // can confirm RP2040→Lyra direction works independently of the beacon.
    if (!lyra_ready) {
        static uint32_t last_hello = 0;
        if (timer_elapsed32(last_hello) >= 2000) {
            uart_write(UART_HID_MAGIC);
            uart_write(0xFE);  // TYPE_HELLO — not a defined HID report
            uart_write(0x00);
            last_hello = timer_read32();
        }
    }

    // 90 s fallback: auto-enable if beacon never arrives.
    if (!lyra_ready && timer_elapsed32(init_time) >= LYRA_READY_TIMEOUT_MS) {
        lyra_ready = true;
        status     = 2;
    }

    return status;
}
// ─────────────────────────────────────────────────────────────────────────────

static void send_frame(uint8_t type, const void *payload, uint8_t len) {
    if (!lyra_ready) return;  // hold until bridge signals it is up
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
    init_time  = timer_read32();
    usb_driver = host_get_driver();
    host_set_driver(&uart_hid_driver);
}
