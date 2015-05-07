#include <pebble.h>

Window *my_window;
TextLayer *time_layer;
TextLayer *text_layer;
GFont time_font;
GFont text_font;

int old_mode;

int h;
int m;

int is_sleep_time(int hour) {
	if(hour < 7)
		return 1;
	
	if(hour >= 23)
		return 1;
	
	return 0;
}

int is_morning(int hour, int minute) {
	if(hour == 7 || (hour == 8 && minute <= 30))
		return 1;

	return 0;
}

int is_go_to_work(int hour, int minute) {
	if(hour == 8 && minute > 30 && minute < 60)
		return 1;

	return 0;
}

int is_do_work(int hour) {
	if(hour >= 9 && hour <= 12)
		return 1;
	
	if(hour >= 14 && hour <= 17)
		return 1;

	return 0;
}

int is_lunch_time(int hour) {
	if(hour == 13)
		return 1;

	return 0;
}

int is_evening(int hour) {
	if(hour >= 18)
		return 1;

	return 0;
}

int is_work_day(int day) {
	if(day == 0 || day == 6)
		return 0;
		
	return 1;
}

int get_mode(int hour, int minute, int day) {
	
	if(is_sleep_time(hour))
		return 0;
		
	if(is_morning(hour, minute))
		return 1;
	
	if(is_work_day(day)) {
		if(is_go_to_work(hour, minute))
			return 2;

		if(is_do_work(hour) && minute <= 30)
			return 3;

		if(is_do_work(hour))
			return 4;
	} else {
		return 6;
	}
	
	if(is_lunch_time(hour))
		return 5;
	
	if(is_evening(hour))
		return 6;
	
	return -1;
}

void set_message(int mode) {
	
	if(mode != old_mode) {
		old_mode = mode;
		vibes_double_pulse();
	}
	
	switch(mode) {
		case 0: 
			text_layer_set_text(text_layer, "Go to\nsleep");
			break;
		
		case 1: 
			text_layer_set_text(text_layer, "Good\nMorning!");
			break;
		
		case 2: 
			text_layer_set_text(text_layer, "Go to\nwork");
			break;
		
		case 3: 
			text_layer_set_text(text_layer, "Do your\nwork");
			break;
		
		case 4: 
			text_layer_set_text(text_layer, "Don't be\nlazy");
			break;
		
		case 5: 
			text_layer_set_text(text_layer, "Eat\nsomething");
			break;
		
		case 6: 
			text_layer_set_text(text_layer, "Relax");
			break;
		
		default:
			return text_layer_set_text(text_layer, "HELLO");
	}
}

void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(time_layer, buffer);
	
	// Display the text message
  
	/* Debug code
	m+=10;
	if(m == 60) {
		m =0;
		h++;
	}
	
	tick_time->tm_hour = h;
	tick_time->tm_min = m;
	*/
	
	strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	
	int mode = get_mode(tick_time->tm_hour, tick_time->tm_min,	tick_time->tm_wday);
	set_message(mode);	
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

void main_window_load(Window *window) {
	// Create time TextLayer
	time_layer = text_layer_create(GRect(0, 150, 144, 20));
	text_layer_set_background_color(time_layer, GColorBlack);
	text_layer_set_text_color(time_layer, GColorClear);
	text_layer_set_text(time_layer, "00:00");
	
	// Create the text layer
	text_layer = text_layer_create(GRect(0, 0, 144, 150));
	text_layer_set_background_color(text_layer, GColorBlack);
	text_layer_set_text_color(text_layer, GColorClear);
	text_layer_set_text(text_layer, "Starting...");
	 
	// Create GFont
	time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_16));
	text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TEXT_36));
	
	// Improve the layout to be more like a watchface
	text_layer_set_font(time_layer, time_font);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	
	text_layer_set_font(text_layer, text_font);
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}

static void main_window_unload(Window *window) {
	// Destroy time font
	fonts_unload_custom_font(time_font);
	fonts_unload_custom_font(text_font);
	
	// Destroy TextLayers
	text_layer_destroy(time_layer);
	text_layer_destroy(text_layer);
}

void handle_init(void) {
  my_window = window_create();
	
	//set window color
	window_set_background_color(my_window, GColorBlack);
	
	// Set handlers to manage the elements inside the Window
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
	
  window_stack_push(my_window, true);
	
	// Make sure the time is displayed from the start
	update_time();
	
	// Register with TickTimerService
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
	h = 0;
	m = 0;
	old_mode = -1;
	
  handle_init();
  app_event_loop();
  handle_deinit();
}
