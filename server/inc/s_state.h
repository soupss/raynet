#ifndef S_STATE_H
#define S_STATE_H

#include "queue.h"

typedef struct SPaddle {
    float pos[2];
    struct lws *wsi;
    Queue *pos_history;
    Queue *pos_dt_history; // dt between recent pos
} SPaddle;

typedef struct SBall {
    float pos[3];
    float vel[3];
    float rot[2];
} SBall;

typedef struct SState {
    SPaddle *p1;
    SPaddle *p2;
    SBall *ball;
} SState;

SState *s_state_create();
void s_state_destroy(SState *s);

#endif
