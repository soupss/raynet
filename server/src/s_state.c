#include <libwebsockets.h>
#include "s_state.h"

SState *s_state_create() {
    SState *s = malloc(sizeof(SState));
    s->p1 = malloc(sizeof(SPaddle));
    s->p2 = malloc(sizeof(SPaddle));
    float zero2[2] = { 0, 0 };
    memcpy(s->p1->pos, zero2, sizeof(zero2));
    memcpy(s->p2->pos, zero2, sizeof(zero2));
    s->p1->wsi = NULL;
    s->p2->wsi = NULL;
    s->p1->pos_history = queue_create();
    s->p2->pos_history = queue_create();
    s->p1->pos_dt_history = queue_create();
    s->p2->pos_dt_history = queue_create();
    s->ball = malloc(sizeof(SBall));
    float zero3[3] = { 0, 0, 0 };
    memcpy(s->ball->pos, zero3, sizeof(zero3));
    memcpy(s->ball->vel, zero3, sizeof(zero3));
    memcpy(s->ball->rot, zero2, sizeof(zero2));
    return s;
}

void s_state_destroy(SState *s) {
    free(s->p1);
    free(s->p2);
    free(s->ball);
    queue_destroy(s->p1->pos_history);
    queue_destroy(s->p1->pos_dt_history);
    queue_destroy(s->p2->pos_history);
    queue_destroy(s->p2->pos_dt_history);
    free(s);
}
