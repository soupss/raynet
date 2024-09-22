#ifndef C_STATE_H
#define C_STATE_H

#include <raylib.h>
#include <emscripten/websocket.h>

#define FPS 0
#define CAMERA_DISTANCE 80
#define CAMERA_FOV 90
#define PADDLE_SPACING 50
#define PADDLE_SIZE 10
#define BALL_RADIUS 2.5
#define BALL_DETAIL 6

typedef struct CState {
    Vector3 player;
    Vector3 enemy;
    Vector3 ball;
    Camera3D camera;
    EMSCRIPTEN_WEBSOCKET_T socket;
} CState;

CState *c_state_create();

#endif
