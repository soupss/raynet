#include "c_state.h"
#include "c_constants.h"
#include "shared_constants.h"

CState *c_state_create() {
    CState *s = malloc(sizeof(CState));
    s->p1 = malloc(sizeof(CPaddle));
    s->p2 = malloc(sizeof(CPaddle));
    s->p1->pos = (Vector3){ OUT_OF_BOUNDS, 0, PADDLE_SPACING };
    s->p2->pos = (Vector3){ OUT_OF_BOUNDS, 0, -PADDLE_SPACING };
    s->p1->alpha = PADDLE_MIN_ALPHA;
    s->p2->alpha = PADDLE_MIN_ALPHA;
    s->ball = (Vector3){ OUT_OF_BOUNDS, 0, 0 };
    s->sfx = c_soundeffects_create();
    s->camera = (Camera3D){ 0 };
    s->camera.position = (Vector3){ 0, 0, CAMERA_DISTANCE };
    s->camera.target = (Vector3){ 0, 0, 0 };
    s->camera.up = (Vector3){ 0, 1, 0 };
    s->camera.fovy = CAMERA_FOV;
    s->camera.projection = CAMERA_PERSPECTIVE;
    s->socket = 0; // needs to be assigned manually
    s->side = SIDE_UNASSIGNED; // needs to be assigned manually
    return s;
}

void c_state_destroy(CState *s) {
    free(s->p1);
    free(s->p2);
    free(s);
}
