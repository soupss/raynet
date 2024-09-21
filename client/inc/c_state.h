#ifndef C_STATE_H
#define C_STATE_H

#include <raylib.h>
#include <emscripten/websocket.h>

#define FPS 0
#define PADDLE_SPACING 20
#define PADDLE_SIZE 10
#define CAMERA_DISTANCE 50
#define CAMERA_FOV 90

typedef struct CState {
    Vector3 player;
    Vector3 enemy;
    Camera3D camera;
    EMSCRIPTEN_WEBSOCKET_T socket;
} CState;

CState *c_state_create();

#endif
