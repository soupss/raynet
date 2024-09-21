#ifndef PLAYER_H
#define PLAYER_H

typedef struct Player {
    float pos[2];
    struct lws *wsi;
} Player;

typedef struct State {
    Player *p1;
    Player *p2;
} State;

State *state_create();

#endif
