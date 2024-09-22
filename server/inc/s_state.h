#ifndef S_STATE_H
#define S_STATE_H

typedef struct SPlayer {
    float pos[2];
    struct lws *wsi;
} SPlayer;

typedef struct SState {
    SPlayer *p1;
    SPlayer *p2;
} SState;

typedef struct SBall {
    float pos[3];
    float speed[3];
    float rotation[3];
} SBall;

typedef struct SWall {
    float a,b,c,d;
} SWall;

SState *s_state_create();

#endif
