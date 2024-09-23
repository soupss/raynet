#ifndef S_STATE_H
#define S_STATE_H

typedef struct SPaddle {
    float pos[2];
    float pos_prev[2];
    double pos_prev_dt;
    float vel[2];
    struct lws *wsi;
} SPaddle;

typedef struct SBall {
    float pos[3];
    float vel[3];
    float curve[2];
} SBall;

typedef struct SState {
    SPaddle *p1;
    SPaddle *p2;
    SBall *ball;
} SState;

SState *s_state_create();

#endif
