#ifndef S_STATE_H
#define S_STATE_H

typedef struct SPlayer {
    float pos[2];
    struct lws *wsi;
} SPlayer;

typedef struct SBall {
    float pos[3];
    //TODO:velocity
    float speed[3];
    float rotation[3];
} SBall;

typedef struct SState {
    SPlayer *p1;
    SPlayer *p2;
    SBall *ball;
} SState;

SState *s_state_create();

#endif
