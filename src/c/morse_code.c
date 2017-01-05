#include <pebble.h>
#include <stdint.h>
#include "morse_code.h"

#define NULL_VIBE get_null_vibe()

static VibePattern* duration_to_vibes(const uint32_t *segments, unsigned int length){
  // lookup table for vibe patterns
  VibePattern* pat = malloc(sizeof(VibePattern));
  pat->durations = segments;
  pat->num_segments = length;
  return pat;
}

static bool first = 0;
VibePattern *null_vibe;
static VibePattern* get_null_vibe(){
  if(first == 0){ // don't wanna waste memory
    null_vibe = duration_to_vibes(0, 0); // single initial allocation
  }
  return null_vibe;
}


// lookup table
VibePattern* ascii_vibes[255] = {
  NULL_VIBE,
  NULL_VIBE,
  
};

VibePattern* string_to_vibe_pattern(const char* ascii){
  for(int i = 0; i < strlen(ascii); i++){
    
  }
  
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  return &pat;
}