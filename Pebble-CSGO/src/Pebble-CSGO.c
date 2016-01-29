#include <pebble.h>

static bool valid_server_ip = false;

static Window *window;
static TextLayer *text_layer;
static TextLayer *t_verbose;

static AppSync s_sync;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
        text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
        text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
        text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
        window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
        window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
        window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
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
}

static void window_unload(Window *window) {
        text_layer_destroy(text_layer);
        text_layer_destroy(t_verbose);
}

static void init(void) {
        window = window_create();
        window_set_click_config_provider(window, click_config_provider);
        window_set_window_handlers(window, (WindowHandlers) {
                        .load = window_load,
                                .unload = window_unload,
                                });
        const bool animated = true;
        window_stack_push(window, animated);

        // Create the configuration buffer
}

static void deinit(void) {
        window_destroy(window);
}

int main(void) {
        init();

        APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

        app_event_loop();
        deinit();
}
