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

#define KEY_SCAN_PRIORITY           8
#define MODE_CTRL_PRIORITY          9
#define LED_EFFECT_PRIORITY         10
#define THREAD_STACK_SIZE           512
#define THREAD_TIMESLICE            5

#define KEY_EVENT_1                 (1u << 0)
#define KEY_EVENT_2                 (1u << 1)
#define KEY_EVENT_3                 (1u << 2)
#define KEY_EVENT_ALL               (KEY_EVENT_1 | KEY_EVENT_2 | KEY_EVENT_3)

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
    (void)parameter;
    while (1)
    {
        rt_thread_mdelay(KEY_SCAN_INTERVAL_MS);
    }
}

static void mode_ctrl_entry(void *parameter)
{
    (void)parameter;
    while (1)
    {
        rt_thread_mdelay(100);
    }
}

static void led_effect_entry(void *parameter)
{
    (void)parameter;
    led_all_off();
    while (1)
    {
        rt_thread_mdelay(100);
    }
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
    return 0;
}
