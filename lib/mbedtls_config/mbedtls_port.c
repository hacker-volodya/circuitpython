/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <py/mpconfig.h>

#if CIRCUITPY_SSL_MBEDTLS

#include "py/runtime.h"
#include "mbedtls_config.h"
#include "mbedtls/entropy_poll.h"

#include "shared/timeutils/timeutils.h"
#include "shared-bindings/os/__init__.h"
#include "shared-bindings/time/__init__.h"

extern uint8_t rosc_random_u8(size_t cycles);

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen) {
    *olen = len;
    common_hal_os_urandom(data, len);
    return 0;
}

time_t rp2_rtctime_seconds(time_t *timer) {
    mp_obj_t datetime = mp_load_attr(MP_STATE_VM(rtc_time_source), MP_QSTR_datetime);
    timeutils_struct_time_t tm;
    struct_time_to_tm(datetime, &tm);
    return timeutils_seconds_since_epoch(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

#endif
