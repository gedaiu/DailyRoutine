#include <pebble.h>

Window *my_window;
TextLayer *time_layer;
GFont time_font;


void main_window_load(Window *window) {
	// Create time TextLayer
	time_layer = text_layer_create(GRect(0, 150, 144, 20));
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_color(time_layer, GColorBlack);
	text_layer_set_text(time_layer, "00:00");
	
	// Create GFont
	time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_16));

	// Improve the layout to be more like a watchface
	text_layer_set_font(time_layer, time_font);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}

static void main_window_unload(Window *window) {
	// Destroy time font
	fonts_unload_custom_font(time_font);
	
	// Destroy TextLayer
	text_layer_destroy(time_layer);
}

void handle_init(void) {
  my_window = window_create();
	
	// Set handlers to manage the elements inside the Window
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
	
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
