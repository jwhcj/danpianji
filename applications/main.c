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
#include "led_app_logic.h"

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

#define KEY_SCAN_PRIORITY           8
#define MODE_CTRL_PRIORITY          9
#define LED_EFFECT_PRIORITY         10
#define THREAD_STACK_SIZE           512
#define THREAD_TIMESLICE            5

#define KEY_EVENT_1                 (1u << 0)
#define KEY_EVENT_2                 (1u << 1)
#define KEY_EVENT_3                 (1u << 2)
#define KEY_EVENT_ALL               (KEY_EVENT_1 | KEY_EVENT_2 | KEY_EVENT_3)

#define LED_EVENT_ALL               (APP_CMD_ALL_ON | APP_CMD_ALL_OFF | \
                                     APP_CMD_FORWARD | APP_CMD_REVERSE)

static struct rt_event key_event;
static struct rt_event led_event;

static rt_thread_t key_scan_thread = RT_NULL;
static rt_thread_t mode_ctrl_thread = RT_NULL;
static rt_thread_t led_effect_thread = RT_NULL;


static void led_write_all(rt_uint8_t level1,
                          rt_uint8_t level2,
                          rt_uint8_t level3)
{
    rt_pin_write(LED1_PIN, level1);
    rt_pin_write(LED2_PIN, level2);
    rt_pin_write(LED3_PIN, level3);
}

static void led_all_on(void)
{
    led_write_all(LED_ON_LEVEL, LED_ON_LEVEL, LED_ON_LEVEL);
}

static void led_all_off(void)
{
    led_write_all(LED_OFF_LEVEL, LED_OFF_LEVEL, LED_OFF_LEVEL);
}

static void led_show_one(rt_uint8_t index)
{
    led_write_all((index == 0u) ? LED_ON_LEVEL : LED_OFF_LEVEL,
                  (index == 1u) ? LED_ON_LEVEL : LED_OFF_LEVEL,
                  (index == 2u) ? LED_ON_LEVEL : LED_OFF_LEVEL);
}

static rt_uint8_t key_read_level(rt_int32_t pin)
{
    return (rt_pin_read(pin) == PIN_HIGH) ? 1u : 0u;
}

static void key_scan_entry(void *parameter)
{
    app_button_filter_t key1_filter;
    app_button_filter_t key2_filter;
    app_button_filter_t key3_filter;
    rt_uint8_t active_level;

    (void)parameter;
    active_level = (KEY_ACTIVE_LEVEL == PIN_HIGH) ? 1u : 0u;

    app_button_filter_init(&key1_filter, key_read_level(KEY1_PIN));
    app_button_filter_init(&key2_filter, key_read_level(KEY2_PIN));
    app_button_filter_init(&key3_filter, key_read_level(KEY3_PIN));

    while (1)
    {
        if (app_button_filter_update(&key1_filter,
                                     key_read_level(KEY1_PIN),
                                     active_level))
        {
            (void)rt_event_send(&key_event, KEY_EVENT_1);
        }

        if (app_button_filter_update(&key2_filter,
                                     key_read_level(KEY2_PIN),
                                     active_level))
        {
            (void)rt_event_send(&key_event, KEY_EVENT_2);
        }

        if (app_button_filter_update(&key3_filter,
                                     key_read_level(KEY3_PIN),
                                     active_level))
        {
            (void)rt_event_send(&key_event, KEY_EVENT_3);
        }

        rt_thread_mdelay(KEY_SCAN_INTERVAL_MS);
    }
}

static unsigned int handle_received_key(app_state_t *state,
                                        rt_uint32_t received)
{
    if ((received & KEY_EVENT_3) != 0u)
    {
        return app_handle_key(state, APP_KEY_3);
    }
    if ((received & KEY_EVENT_1) != 0u)
    {
        return app_handle_key(state, APP_KEY_1);
    }
    if ((received & KEY_EVENT_2) != 0u)
    {
        return app_handle_key(state, APP_KEY_2);
    }
    return APP_CMD_NONE;
}

static void print_mode_change(const app_state_t *state, unsigned int command)
{
    if (command == APP_CMD_ALL_ON)
    {
        rt_kprintf("[mode] key3 count=1, all LEDs on\n");
    }
    else if ((command == APP_CMD_ALL_OFF) &&
             (state->mode == APP_MODE_FLOW_READY))
    {
        rt_kprintf("[mode] key3 count=2, flow mode ready\n");
    }
    else if (command == APP_CMD_ALL_OFF)
    {
        rt_kprintf("[mode] key3 count=0, all LEDs off\n");
    }
    else if (command == APP_CMD_FORWARD)
    {
        rt_kprintf("[flow] forward: LED1 -> LED2 -> LED3\n");
    }
    else if (command == APP_CMD_REVERSE)
    {
        rt_kprintf("[flow] reverse: LED3 -> LED2 -> LED1\n");
    }
}

static void mode_ctrl_entry(void *parameter)
{
    app_state_t state;
    rt_uint32_t received;
    unsigned int command;
    rt_err_t result;

    (void)parameter;
    app_state_init(&state);

    while (1)
    {
        result = rt_event_recv(&key_event,
                               KEY_EVENT_ALL,
                               RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                               RT_WAITING_FOREVER,
                               &received);
        if (result != RT_EOK)
        {
            rt_kprintf("[error] receive key event failed: %d\n", result);
            continue;
        }

        command = handle_received_key(&state, received);
        if (command != APP_CMD_NONE)
        {
            print_mode_change(&state, command);
            result = rt_event_send(&led_event, command);
            if (result != RT_EOK)
            {
                rt_kprintf("[error] send LED event failed: %d\n", result);
            }
        }
    }
}

static void led_effect_entry(void *parameter)
{
    app_state_t effect_state;
    rt_uint32_t received;
    rt_int32_t timeout;
    rt_err_t result;
    int running;

    (void)parameter;
    app_state_init(&effect_state);
    running = 0;
    led_all_off();

    while (1)
    {
        timeout = running
                ? (rt_int32_t)rt_tick_from_millisecond(FLOW_INTERVAL_MS)
                : RT_WAITING_FOREVER;

        result = rt_event_recv(&led_event,
                               LED_EVENT_ALL,
                               RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                               timeout,
                               &received);

        if (result == RT_EOK)
        {
            if ((received & APP_CMD_ALL_OFF) != 0u)
            {
                running = 0;
                app_state_init(&effect_state);
                led_all_off();
            }
            else if ((received & APP_CMD_ALL_ON) != 0u)
            {
                running = 0;
                effect_state.mode = APP_MODE_ALL_ON;
                led_all_on();
            }
            else if ((received & APP_CMD_FORWARD) != 0u)
            {
                running = 1;
                effect_state.mode = APP_MODE_FLOW_FORWARD;
                effect_state.flow_index = 0u;
                led_show_one(app_current_led(&effect_state));
            }
            else if ((received & APP_CMD_REVERSE) != 0u)
            {
                running = 1;
                effect_state.mode = APP_MODE_FLOW_REVERSE;
                effect_state.flow_index = 2u;
                led_show_one(app_current_led(&effect_state));
            }
        }
        else if ((result == -RT_ETIMEOUT) && running)
        {
            app_advance_led(&effect_state);
            led_show_one(app_current_led(&effect_state));
        }
        else
        {
            rt_kprintf("[error] receive LED event failed: %d\n", result);
            rt_thread_mdelay(10);
        }
    }
}

static void delete_created_threads(void)
{
    if (key_scan_thread != RT_NULL)
    {
        (void)rt_thread_delete(key_scan_thread);
        key_scan_thread = RT_NULL;
    }
    if (mode_ctrl_thread != RT_NULL)
    {
        (void)rt_thread_delete(mode_ctrl_thread);
        mode_ctrl_thread = RT_NULL;
    }
    if (led_effect_thread != RT_NULL)
    {
        (void)rt_thread_delete(led_effect_thread);
        led_effect_thread = RT_NULL;
    }
}

static int create_threads(void)
{
    key_scan_thread = rt_thread_create("key_scan", key_scan_entry, RT_NULL,
                                       THREAD_STACK_SIZE, KEY_SCAN_PRIORITY,
                                       THREAD_TIMESLICE);
    if (key_scan_thread == RT_NULL)
    {
        rt_kprintf("[error] create key_scan thread failed\n");
        return -1;
    }

    mode_ctrl_thread = rt_thread_create("mode_ctrl", mode_ctrl_entry, RT_NULL,
                                        THREAD_STACK_SIZE, MODE_CTRL_PRIORITY,
                                        THREAD_TIMESLICE);
    if (mode_ctrl_thread == RT_NULL)
    {
        rt_kprintf("[error] create mode_ctrl thread failed\n");
        delete_created_threads();
        return -1;
    }

    led_effect_thread = rt_thread_create("led_effect", led_effect_entry,
                                         RT_NULL, THREAD_STACK_SIZE,
                                         LED_EFFECT_PRIORITY,
                                         THREAD_TIMESLICE);
    if (led_effect_thread == RT_NULL)
    {
        rt_kprintf("[error] create led_effect thread failed\n");
        delete_created_threads();
        return -1;
    }
    return 0;
}


static int start_threads(void)
{
    if (rt_thread_startup(led_effect_thread) != RT_EOK)
    {
        rt_kprintf("[error] start led_effect thread failed\n");
        return -1;
    }
    if (rt_thread_startup(mode_ctrl_thread) != RT_EOK)
    {
        rt_kprintf("[error] start mode_ctrl thread failed\n");
        return -1;
    }
    if (rt_thread_startup(key_scan_thread) != RT_EOK)
    {
        rt_kprintf("[error] start key_scan thread failed\n");
        return -1;
    }
    return 0;
}


int main(void)
{
    rt_err_t result;

    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    led_all_off();

    rt_pin_mode(KEY1_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY2_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY3_PIN, PIN_MODE_INPUT_PULLUP);

    result = rt_event_init(&key_event, "key_evt", RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("[error] initialize key_event failed: %d\n", result);
        return -1;
    }

    result = rt_event_init(&led_event, "led_evt", RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("[error] initialize led_event failed: %d\n", result);
        return -1;
    }


    if (create_threads() != 0)
    {
        return -1;
    }

    if (start_threads() != 0)
    {
        return -1;
    }

        rt_kprintf("[app] three-key LED event demo started\n");


    return 0;
}
