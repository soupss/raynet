#ifndef C_STATE_H
#define C_STATE_H

#include <raylib.h>
#include <emscripten/websocket.h>

typedef enum CPLAYER_SIDE {
    SIDE_UNASSIGNED = 0,
    SIDE_1 = 1,
    SIDE_2 = 2
} CPLAYER_SIDE;

typedef struct CState {
    CPLAYER_SIDE side;
    Vector3 player1;
    Vector3 player2;
    Vector3 ball;
    Camera3D camera;
    EMSCRIPTEN_WEBSOCKET_T socket;
} CState;

CState *c_state_create();

#endif
