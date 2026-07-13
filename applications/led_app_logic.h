#ifndef LED_APP_LOGIC_H
#define LED_APP_LOGIC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    APP_MODE_OFF = 0,
    APP_MODE_ALL_ON,
    APP_MODE_FLOW_READY,
    APP_MODE_FLOW_FORWARD,
    APP_MODE_FLOW_REVERSE
} app_mode_t;

typedef enum
{
    APP_KEY_NONE = 0,
    APP_KEY_1,
    APP_KEY_2,
    APP_KEY_3
} app_key_t;

#define APP_CMD_NONE       0u
#define APP_CMD_ALL_ON     (1u << 0)
#define APP_CMD_ALL_OFF    (1u << 1)
#define APP_CMD_FORWARD    (1u << 2)
#define APP_CMD_REVERSE    (1u << 3)

typedef struct
{
    app_mode_t mode;
    unsigned char key3_count;
    unsigned char flow_index;
} app_state_t;

void app_state_init(app_state_t *state);
unsigned int app_handle_key(app_state_t *state, app_key_t key);
unsigned char app_current_led(const app_state_t *state);
void app_advance_led(app_state_t *state);

#ifdef __cplusplus
}
#endif

#endif