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

SState *s_state_create();

#endif
