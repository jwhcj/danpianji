#include "led_app_logic.h"

void app_state_init(app_state_t *state)
{
    state->mode = APP_MODE_OFF;
    state->key3_count = 0u;
    state->flow_index = 0u;
}

unsigned int app_handle_key(app_state_t *state, app_key_t key)
{
    if (key != APP_KEY_3)
    {
        return APP_CMD_NONE;
    }

    if (state->key3_count == 0u)
    {
        state->key3_count = 1u;
        state->mode = APP_MODE_ALL_ON;
        return APP_CMD_ALL_ON;
    }

    if (state->key3_count == 1u)
    {
        state->key3_count = 2u;
        state->mode = APP_MODE_FLOW_READY;
        state->flow_index = 0u;
        return APP_CMD_ALL_OFF;
    }

    app_state_init(state);
    return APP_CMD_ALL_OFF;
}