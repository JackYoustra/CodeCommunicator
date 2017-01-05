#include <pebble.h>
#include "morse_code.h"

static Window *mainWindow;
static TextLayer *instructions[3];
static GRect windowBounds;
static DictationSession *s_dictation_session;
static ScrollLayer *dictationScrollLayer;
static TextLayer *dictationScrollLayerTextLayer; // with text
static TextLayer *dictationScrollLayerTextLayerSpoken; // with grey text over so that it looks a different color

// Declare a buffer for the DictationSession, making it p big
static char s_last_text[4096];

// use this function to free memory
static void window_unload(Window *window){
  //We will safely destroy the Window's elements here!
  for(int i = 0; i < 3; i++){
    text_layer_destroy(instructions[i]);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context){
  // clear output buffer
  
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // record to output buffer
  // Start dictation UI
  dictation_session_start(s_dictation_session);  
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context){
  // pause / resume button
  
}

static void vibrate_character(const char character){
  
}

static bool done = 1;
static int current_position = 0;
static void text_update(){
  if(done){
    // completed with current one, begin a new cycle
    // get text at current position
    const char *text = text_layer_get_text(dictationScrollLayerTextLayer);
    char current_character  = text[current_position];
    vibrate_character(current_character);
  }
}

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Dictation status: %d", (int)status);
  if(status == DictationSessionStatusSuccess) {
    // Display the dictated text
    snprintf(s_last_text, sizeof(s_last_text), "Transcription:\n\n%s", transcription);
    
    // concatenate current and other strings
    const char *layer_text = text_layer_get_text(dictationScrollLayerTextLayer); 
    char dictation_text[strlen(layer_text) + 4096]; // create combined buffer
    strcpy(dictation_text, layer_text);
    const char *final_string = strcat(dictation_text, s_last_text);
    text_layer_set_text(dictationScrollLayerTextLayer, final_string);
    
    // now update sizes of frames
    const GSize final_size = text_layer_get_content_size(dictationScrollLayerTextLayer);
    scroll_layer_set_content_size(dictationScrollLayer, final_size);
    text_layer_set_size(dictationScrollLayerTextLayer, final_size);
    text_layer_set_size(dictationScrollLayerTextLayerSpoken, final_size);
    text_update();
    
  } else {
    // Display the reason for any error
    static char s_failed_buff[128];
    snprintf(s_failed_buff, sizeof(s_failed_buff), "Transcription failed.\n\nReason:\n%d", (int)status);
    APP_LOG(APP_LOG_LEVEL_ERROR, s_failed_buff);
  }
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window){
  VibePattern *pattern = string_to_vibe_pattern("s");
  const char* messages[] = {
    "X", // clear (backspace)
    "\u2022", // record (pip)
    "||" // pause / play (pause now)
  };
  windowBounds = layer_get_bounds(window_get_root_layer(mainWindow));
  const int16_t third_height = windowBounds.size.h/3;
  const int16_t tenth_width = windowBounds.size.w/10;
  // do common processing
  for(uint8_t i = 0; i < 3; i++){
    instructions[i] = text_layer_create(GRect(tenth_width*9, third_height*i, tenth_width, third_height));
    text_layer_set_text(instructions[i], messages[i]);
    text_layer_set_background_color(instructions[i], GColorClear);
    text_layer_set_text_color(instructions[i], GColorBlack);
    text_layer_set_font(instructions[i], fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(instructions[i], GTextAlignmentRight);
    
    layer_add_child(window_get_root_layer(mainWindow), text_layer_get_layer(instructions[i]));
  }
  
  // Create new dictation session
  s_dictation_session = dictation_session_create(sizeof(s_last_text), dictation_session_callback, NULL);
  
  dictationScrollLayer = scroll_layer_create(GRect(0, 0, tenth_width*9, windowBounds.size.h));
  dictationScrollLayerTextLayer = text_layer_create(GRect(0, 0, tenth_width*9, 0)); // zero height initially, resize on callback
  dictationScrollLayerTextLayerSpoken = text_layer_create(GRect(0, 0, tenth_width*9, 0));
  text_layer_set_background_color(dictationScrollLayerTextLayerSpoken, GColorClear);
  text_layer_set_text_color(dictationScrollLayerTextLayerSpoken, GColorLightGray);
  
  scroll_layer_add_child(dictationScrollLayer, (Layer*)dictationScrollLayerTextLayer);
  scroll_layer_set_content_size(dictationScrollLayer, GSize(tenth_width*9, 0));
}

void init(){
  //Create app elements here
  // make main window
  mainWindow = window_create();
  window_set_window_handlers(mainWindow, (WindowHandlers) {
    // delegate window functions / hooks
    .load = window_load,
    .unload = window_unload,
  });
  
  // make app move to foreground
  window_set_click_config_provider(mainWindow, click_config_provider);
  window_stack_push(mainWindow, true);
}

void deinit(){
  //We will safely destroy the Window's elements here!
  window_destroy(mainWindow);
}


int main(){
  init();
  app_event_loop();
  deinit();
}