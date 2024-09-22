#include "c_state.h"

CState *c_state_create() {
    CState *s = malloc(sizeof(CState));
    s->player = (Vector3){ 0, 0, PADDLE_SPACING };
    s->enemy = (Vector3){ 0, 0, -PADDLE_SPACING };
    s->ball = (Vector3){ 0, 0, 0 }; //offscreen
    s->camera = (Camera3D){ 0 };
    s->camera.position = (Vector3){ 0, 0, CAMERA_DISTANCE };
    s->camera.target = (Vector3){ 0, 0, 0 };
    s->camera.up = (Vector3){ 0, 1, 0 };
    s->camera.fovy = CAMERA_FOV;
    s->camera.projection = CAMERA_PERSPECTIVE;
    s->socket = 0; // needs to be assigned manually
    return s;
}
