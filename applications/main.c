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

#define LED_PIN	3

static void test_forward_flow(void)
{
    app_state_t state;

    app_state_init(&state);
    (void)app_handle_key(&state, APP_KEY_3);
    (void)app_handle_key(&state, APP_KEY_3);
    assert(app_handle_key(&state, APP_KEY_1) == APP_CMD_FORWARD);
    assert(app_current_led(&state) == 0u);
    app_advance_led(&state);
    assert(app_current_led(&state) == 1u);
    app_advance_led(&state);
    assert(app_current_led(&state) == 2u);
    app_advance_led(&state);
    assert(app_current_led(&state) == 0u);
}

static void test_reverse_flow(void)
{
    app_state_t state;

    app_state_init(&state);
    (void)app_handle_key(&state, APP_KEY_3);
    (void)app_handle_key(&state, APP_KEY_3);
    assert(app_handle_key(&state, APP_KEY_2) == APP_CMD_REVERSE);
    assert(app_current_led(&state) == 2u);
    app_advance_led(&state);
    assert(app_current_led(&state) == 1u);
    app_advance_led(&state);
    assert(app_current_led(&state) == 0u);
    app_advance_led(&state);
    assert(app_current_led(&state) == 2u);
}

int main(void)
{
    test_mode_cycle();
    test_forward_flow();
    test_reverse_flow();
    return 0;
}

int led(void)
{
    rt_uint8_t count;

    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);  
    
    for(count = 0 ; count < 10 ;count++)
    {       
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_kprintf("led on, count : %d\r\n", count);
        rt_thread_mdelay(500);
        
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_kprintf("led off\r\n");
        rt_thread_mdelay(500);
    }
    return 0;
}
MSH_CMD_EXPORT(led, RT-Thread first led sample);
