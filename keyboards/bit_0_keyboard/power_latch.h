// Copyright 2024 PepeEdwards
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>

void power_latch_init(uint32_t pin);
void power_latch_close(void);
void power_latch_open(void);
bool power_latch_is_closed(void);
void power_latch_task(void);
