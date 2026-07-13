#include "led_app_logic.h"

void app_state_init(app_state_t *state)
{
    state->mode = APP_MODE_OFF;
    state->key3_count = 0u;
    state->flow_index = 0u;
}

unsigned int app_handle_key(app_state_t *state, app_key_t key)
{
    if ((key == APP_KEY_1) && (state->key3_count == 2u))
    {
        state->mode = APP_MODE_FLOW_FORWARD;
        state->flow_index = 0u;
        return APP_CMD_FORWARD;
    }

    if ((key == APP_KEY_2) && (state->key3_count == 2u))
    {
        state->mode = APP_MODE_FLOW_REVERSE;
        state->flow_index = 2u;
        return APP_CMD_REVERSE;
    }

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

unsigned char app_current_led(const app_state_t *state)
{
    return state->flow_index;
}

void app_advance_led(app_state_t *state)
{
    if (state->mode == APP_MODE_FLOW_FORWARD)
    {
        state->flow_index = (unsigned char)((state->flow_index + 1u) % 3u);
    }
    else if (state->mode == APP_MODE_FLOW_REVERSE)
    {
        state->flow_index = (state->flow_index == 0u)
                          ? 2u
                          : (unsigned char)(state->flow_index - 1u);
    }
}

void app_button_filter_init(app_button_filter_t *filter,
                            unsigned char initial_level)
{
    filter->stable_level = initial_level;
    filter->candidate_level = initial_level;
    filter->candidate_count = 0u;
}

int app_button_filter_update(app_button_filter_t *filter,
                             unsigned char raw_level,
                             unsigned char active_level)
{
    if (raw_level == filter->stable_level)
    {
        filter->candidate_level = raw_level;
        filter->candidate_count = 0u;
        return 0;
    }

    if (raw_level != filter->candidate_level)
    {
        filter->candidate_level = raw_level;
        filter->candidate_count = 1u;
        return 0;
    }

    if (filter->candidate_count < 2u)
    {
        filter->candidate_count++;
    }

    if (filter->candidate_count >= 2u)
    {
        filter->stable_level = raw_level;
        filter->candidate_count = 0u;
        return (raw_level == active_level) ? 1 : 0;
    }
    return 0;
}