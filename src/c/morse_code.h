// http://morsecode.scphillips.com/morse2.html
#include <pebble.h>

#define UNIT_LENGTH 100

#define DOT UNIT_LENGTH
#define DASH UNIT_LENGTH*3

#define INTERNAL_PAUSE UNIT_LENGTH
#define CHARACTER_PAUSE UNIT_LENGTH*3
#define WORD_PAUSE UNIT_LENGTH*7

#define VIBE_PATTERN(...) {.durations = { __VA_ARGS__ }, .num_segments = ARRAY_LENGTH(.durations),}

VibePattern* string_to_vibe_pattern(const char* ascii);