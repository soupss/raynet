#include <stdlib.h>
#include "c_sound.h"

SoundEffects *c_soundeffects_create() {
    SoundEffects *sfx = malloc(sizeof(SoundEffects));
    // sfx->ball_hit_wall = LoadSound("../resources/ball_hit_wall.wav");
    sfx->ball_hit_paddle = LoadSound("../resources/ball_hit_paddle.wav");
    // sfx->ball_out = LoadSound("../resources/ball_out.wav");
    return sfx;
}
