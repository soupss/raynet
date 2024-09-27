#ifndef C_STATE_H
#define C_STATE_H

#include <raylib.h>
#include <emscripten/websocket.h>
#include "c_sound.h"
#include "shared_constants.h"

typedef struct CPaddle {
    Vector3 pos;
    float alpha;
} CPaddle;

typedef struct CState {
    PADDLE_SIDE side;
    CPaddle *paddle1; //TODO: change to p1
    CPaddle *paddle2;
    Vector3 ball;
    Camera3D camera;
    SoundEffects *sfx;
    EMSCRIPTEN_WEBSOCKET_T socket;
} CState;

CState *c_state_create();
void c_state_destroy(CState *s);

#endif
