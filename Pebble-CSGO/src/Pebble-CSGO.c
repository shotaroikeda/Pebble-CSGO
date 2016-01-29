/* Include definitions/constants */ 
#include "Pebble-CSGO.h"

#include <pebble.h>
#include <string.h>



static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
        /***************************************************************************************************/
        /* Notes on callback:                                                                              */
        /*      Seems like the call back gets called in streams of 2. In other words, we need to           */
        /*      "drop" very other value, or else the buzzing vibration doesn't work as well.               */
        /***************************************************************************************************/

        switch (key) {
        case CSGO_VALID_SERVER_IP:
                valid_server_ip = new_tuple->value->uint8;
                valid_server_ip ?
                        text_layer_set_text(t_verbose, "All is good.") :
                        text_layer_set_text(t_verbose, "No Valid server found.");
                // APP_LOG(APP_LOG_LEVEL_DEBUG, "Valid_server_ip: %d", new_tuple->value->uint8);
                break;

        case CSGO_TIME_SINCE_LAST_UPDATE:
                // APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Since Last Update: %d", new_tuple->value->uint8);
                break;

        case CSGO_MAP_MODE:
                // APP_LOG(APP_LOG_LEVEL_DEBUG, "CSGO Mode: %s", new_tuple->value->cstring);
                break;

        case CSGO_ROUND_PHASE:
                // APP_LOG(APP_LOG_LEVEL_DEBUG, "CSGO Round Phase: %s", new_tuple->value->cstring);

                // We want to take the first available input then drop the 2nd one. i.e.
                // drop = !drop should come after this portion
                if (drop_flg &&
                    (strcmp(new_tuple->value->cstring, "freezetime")     == 0 ||
                     strcmp(new_tuple->value->cstring, "over")           == 0)) {
                        vibes_long_pulse();
                        waiter = WAIT_TIME;
                } else {
                        // APP_LOG(APP_LOG_LEVEL_DEBUG, "%s != freezetime", new_tuple->value->cstring);
                }
                drop_flg = !drop_flg;

                break;

        case CSGO_BOMB_STATUS:
                // APP_LOG(APP_LOG_LEVEL_DEBUG, "CSGO Bomb Status: %s", new_tuple->value->cstring);

                /* if (strcmp(new_tuple->value->cstring, "exploded") == 0 || */
                /*     strcmp(new_tuple->value->cstring, "defused") == 0 || */
                /*      ) { */
                /*         vibes_long_pulse(); */
                /*      ; */
                /*         waiter = WAIT_TIME; */
                /* } */
                break;

	case CSGO_TEAM_CT_SCORE:


        default:
                break;
        }
}

static void request_csgo_data() {
        // No message passed, send ping data
        DictionaryIterator * it;
        app_message_outbox_begin(&it);

        if (!it) return;

        int value = 1;
        dict_write_int(it, 1, &value, sizeof(int), true);
        dict_write_end(it);

        app_message_outbox_send();
}

static void tick_timer_second_handler(struct tm *tick_time, TimeUnits units_changed)
{
        // Refresh every 5 seconds
        if (tick_time->tm_sec % REFRESH_INTERVAL == 0 && waiter == 0)
        {
                request_csgo_data();
        }
        else if (waiter != 0)
        {
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Waiter Enabled: %d", waiter);
                waiter--;
        }
}

static void window_load(Window *window) {
        Layer *window_layer = window_get_root_layer(window);
        GRect bounds = layer_get_bounds(window_layer);

        text_layer = text_layer_create((GRect) { .origin = { 0, 36 }, .size = { bounds.size.w, 20 } });
        t_verbose = text_layer_create((GRect) { .origin = { 0, 108 }, .size = { bounds.size.w, 20 } });
        text_layer_set_text(text_layer, "Press a button");

        valid_server_ip ?
                text_layer_set_text(t_verbose, "All is good.") :
                text_layer_set_text(t_verbose, "No Valid server found.");

        text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
        text_layer_set_text_alignment(t_verbose, GTextAlignmentCenter);

        layer_add_child(window_layer, text_layer_get_layer(text_layer));
        layer_add_child(window_layer, text_layer_get_layer(t_verbose));

        Tuplet initial_vals[] = {
                TupletInteger(CSGO_VALID_SERVER_IP, (uint8_t) 0),
                TupletInteger(CSGO_TIME_SINCE_LAST_UPDATE, (uint8_t)0),
                TupletCString(CSGO_MAP_MODE, "none"),
                TupletCString(CSGO_ROUND_PHASE, "none"),
                TupletCString(CSGO_BOMB_STATUS, "none")
                TupletInteger(CSGO_TEAM_CT_SCORE, (uint8_t)0),
                TupletInteger(CSGO_TEAM_T_SCORE, (uint8_t)0),
        };

        app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer),
                      initial_vals, ARRAY_LENGTH(initial_vals),
                      sync_tuple_changed_callback, sync_error_callback, NULL);

        request_csgo_data();
}

static void window_unload(Window *window) {
        text_layer_destroy(text_layer);
        text_layer_destroy(t_verbose);
}

static void init(void) {
        // Subscribe to tick handler
        tick_timer_service_subscribe(SECOND_UNIT, tick_timer_second_handler);

        window = window_create();
        window_set_window_handlers(window, (WindowHandlers) {
                        .load = window_load,
                                .unload = window_unload,
                                });
        const bool animated = true;
        window_stack_push(window, animated);

        // Create the configuration buffer
        app_message_open(128, 64);
}

static void deinit(void) {
        tick_timer_service_unsubscribe();
        window_destroy(window);
        app_sync_deinit(&s_sync);
}

int main(void) {
        init();

        // APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

        app_event_loop();
        deinit();
}
