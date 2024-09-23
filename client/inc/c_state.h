#ifndef C_STATE_H
#define C_STATE_H

#include <raylib.h>
#include <emscripten/websocket.h>
#include "shared_constants.h"

typedef struct CState {
    PADDLE_SIDE side;
    Vector3 paddle1;
    Vector3 paddle2;
    float p1_alpha;
    float p2_alpha;
    Vector3 ball;
    Camera3D camera;
    EMSCRIPTEN_WEBSOCKET_T socket;
} CState;

CState *c_state_create();

#endif
