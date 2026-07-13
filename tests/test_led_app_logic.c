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

int main(void)
{
    test_mode_cycle();
    return 0;
}