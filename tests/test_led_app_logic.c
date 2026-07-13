#include <assert.h>
#include "led_app_logic.h"

static void test_mode_cycle(void)
{
    app_state_t state;

    app_state_init(&state);
    assert(state.mode == APP_MODE_OFF);
    assert(state.key3_count == 0u);
    assert(app_handle_key(&state, APP_KEY_1) == APP_CMD_NONE);
    assert(app_handle_key(&state, APP_KEY_2) == APP_CMD_NONE);

    assert(app_handle_key(&state, APP_KEY_3) == APP_CMD_ALL_ON);
    assert(state.mode == APP_MODE_ALL_ON);
    assert(state.key3_count == 1u);

    assert(app_handle_key(&state, APP_KEY_3) == APP_CMD_ALL_OFF);
    assert(state.mode == APP_MODE_FLOW_READY);
    assert(state.key3_count == 2u);

    assert(app_handle_key(&state, APP_KEY_3) == APP_CMD_ALL_OFF);
    assert(state.mode == APP_MODE_OFF);
    assert(state.key3_count == 0u);
}

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