#include <pebble.h>
#include <string.h>

static bool valid_server_ip = false;
static int waiter = 0;

static Window *window;
static TextLayer *text_layer;
static TextLayer *t_verbose;

static AppSync s_sync;
static uint8_t s_sync_buffer[128];

// Register enumerators for better syntax
enum CSGO_KEYS {
        CSGO_VALID_SERVER_IP        = 0,
        CSGO_TIME_SINCE_LAST_UPDATE = 1,
        CSGO_MAP_MODE               = 2,
        CSGO_ROUND_PHASE            = 3,
        CSGO_BOMB_STATUS            = 4
};

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
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

		if (strcmp(new_tuple->value->cstring, "freezetime") == 0) {
			vibes_short_pulse();
			vibes_short_pulse();
			waiter = 20;
		} else {
			// APP_LOG(APP_LOG_LEVEL_DEBUG, "%s != freezetime", new_tuple->value->cstring);
		}

		break;

	case CSGO_BOMB_STATUS:
		// // APP_LOG(APP_LOG_LEVEL_DEBUG, "CSGO Bomb Status: %s", new_tuple->value->cstring);

		if (strcmp(new_tuple->value->cstring, "exploded") == 0 ||
		    strcmp(new_tuple->value->cstring, "defused") == 0) {
			vibes_short_pulse();
			vibes_short_pulse();
			waiter = 20;
		}
		break;

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
	// Refresh every other second
	if (tick_time->tm_sec % 2 == 0 || waiter == 0)
	{
		request_csgo_data();
	} else if (waiter != 0) {
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
