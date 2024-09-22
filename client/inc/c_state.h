#ifndef C_STATE_H
#define C_STATE_H

#include <raylib.h>
#include <emscripten/websocket.h>

typedef struct CState {
    int role; //1 player1, 0 nothing, -1 player2
    Vector3 player1;
    Vector3 player2;
    Vector3 ball;
    Camera3D camera;
    EMSCRIPTEN_WEBSOCKET_T socket;
} CState;

CState *c_state_create();

#endif
