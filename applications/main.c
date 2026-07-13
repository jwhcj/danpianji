/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-16     armink       first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>

#define LED1_PIN                    1
#define LED2_PIN                    2
#define LED3_PIN                    3

#define KEY1_PIN                    10
#define KEY2_PIN                    11
#define KEY3_PIN                    12

#define KEY_ACTIVE_LEVEL            PIN_LOW
#define LED_ON_LEVEL                PIN_HIGH
#define LED_OFF_LEVEL               PIN_LOW

#define KEY_SCAN_INTERVAL_MS        20
#define FLOW_INTERVAL_MS            300

int main(void)
{
    return 0;
}
