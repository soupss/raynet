#ifndef S_STATE_H
#define S_STATE_H

typedef struct SPaddle {
    float pos[2];
    struct lws *wsi;
} SPaddle;

typedef struct SBall {
    float pos[3];
    float vel[3];
} SBall;

typedef struct SState {
    SPaddle *p1;
    SPaddle *p2;
    SBall *ball;
} SState;

SState *s_state_create();

#endif
