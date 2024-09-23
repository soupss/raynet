#include <libwebsockets.h>
#include "s_state.h"

SState *s_state_create() {
    SState *s = malloc(sizeof(SState));
    s->p1 = malloc(sizeof(SPaddle));
    s->p2 = malloc(sizeof(SPaddle));
    float zero2[2] = { 0, 0 };
    memcpy(s->p1->pos, zero2, sizeof(zero2));
    memcpy(s->p2->pos, zero2, sizeof(zero2));
    memcpy(s->p1->pos_prev, zero2, sizeof(zero2));
    memcpy(s->p2->pos_prev, zero2, sizeof(zero2));
    memcpy(s->p1->vel, zero2, sizeof(zero2));
    memcpy(s->p2->vel, zero2, sizeof(zero2));
    s->p1->pos_prev_dt = 0;
    s->p2->pos_prev_dt = 0;
    s->p1->wsi = NULL;
    s->p2->wsi = NULL;
    s->ball = malloc(sizeof(SBall));
    float zero3[3] = { 0, 0, 0 };
    memcpy(s->ball->pos, zero3, sizeof(zero3));
    memcpy(s->ball->vel, zero3, sizeof(zero3));
    memcpy(s->ball->curve, zero2, sizeof(zero2));
    return s;
}
