#ifndef SOUND_H
#define SOUND_H

#include <raylib.h>

typedef struct SoundEffects {
    Sound ball_hit_wall;
    Sound ball_hit_paddle;
    Sound ball_out;
} SoundEffects;

SoundEffects *c_soundeffects_create();
void c_soundeffects_destroy(SoundEffects *sfx);

#endif
