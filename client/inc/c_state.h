#ifndef C_STATE_H
#define C_STATE_H

#include <raylib.h>
#include <emscripten/websocket.h>
#include "shared_constants.h"

typedef struct CPaddle {
    Vector3 pos;
    float alpha;
} CPaddle;

typedef struct CState {
    PADDLE_SIDE side;
    CPaddle *paddle1;
    CPaddle *paddle2;
    Vector3 ball;
    Camera3D camera;
    EMSCRIPTEN_WEBSOCKET_T socket;
} CState;

CState *c_state_create();

#endif
