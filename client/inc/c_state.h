#ifndef C_STATE_H
#define C_STATE_H

#include <raylib.h>
#include <emscripten/websocket.h>
#include "shared_constants.h"

typedef struct CState {
    PLAYER_SIDE side;
    Vector3 player1;
    Vector3 player2;
    float p1_alpha;
    float p2_alpha;
    Vector3 ball;
    Camera3D camera;
    EMSCRIPTEN_WEBSOCKET_T socket;
} CState;

CState *c_state_create();

#endif
