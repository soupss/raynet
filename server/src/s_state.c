#include <libwebsockets.h>
#include "s_state.h"

SState *s_state_create() {
    SState *s = malloc(sizeof(SState));
    s->p1 = malloc(sizeof(SPaddle));
    s->p2 = malloc(sizeof(SPaddle));
    float pos[2] = {0, 0};
    memcpy(s->p1->pos, pos, sizeof(pos));
    memcpy(s->p2->pos, pos, sizeof(pos));
    s->p1->wsi = NULL;
    s->p2->wsi = NULL;
    s->ball = malloc(sizeof(SBall));
    float zero[3] = {0,0,0};
    memcpy(s->ball->pos, zero, sizeof(zero));
    memcpy(s->ball->vel, zero, sizeof(zero));
    return s;
}
