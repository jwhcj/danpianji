#include <assert.h>
#include "led_app_logic.h"

int main(void)
{
    app_state_t state;
    state.mode = APP_MODE_OFF;
    assert(APP_CMD_ALL_ON != APP_CMD_ALL_OFF);
    return 0;
}