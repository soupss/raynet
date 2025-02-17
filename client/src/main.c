#include <raylib.h>
#include <rlgl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <emscripten/websocket.h>
#include "c_websocket.h"
#include "c_state.h"
#include "c_sound.h"
#include "c_constants.h"
#include "shared_constants.h"

static void _update(CState *s) {
    Ray mouse = GetMouseRay(GetMousePosition(), s->camera);
    float z = PADDLE_SPACING;
    if (s->side == SIDE_2) {
        z *= -1;
    }
    float d = (z - mouse.position.z) / mouse.direction.z; // distance from camera to paddle
    float pos[2] = {mouse.position.x + mouse.direction.x * d,
                    mouse.position.y + mouse.direction.y * d};
    float x_min = -ARENA_WIDTH / 2.0 + PADDLE_WIDTH / 2.0;
    float x_max = ARENA_WIDTH / 2.0 - PADDLE_WIDTH / 2.0;
    if (pos[0] < x_min) {
        pos[0] = x_min;
    }
    else if (pos[0] > x_max) {
        pos[0] = x_max;
    }
    float y_min = -ARENA_HEIGHT / 2.0 + PADDLE_HEIGHT / 2.0;
    float y_max = ARENA_HEIGHT / 2.0 - PADDLE_HEIGHT / 2.0;
    if (pos[1] < y_min) {
        pos[1] = y_min;
    }
    else if (pos[1] > y_max) {
        pos[1] = y_max;
    }
    c_ws_send_paddle_state(s->socket, pos);
    if (s->side == SIDE_1) {
        s->p1->pos.x = pos[0];
        s->p1->pos.y = pos[1];
    }
    else if (s->side == SIDE_2) {
        s->p2->pos.x = pos[0];
        s->p2->pos.y = pos[1];
    }
}

static void _draw(CState *s) {
    Vector3 paddle_size = { PADDLE_WIDTH, PADDLE_HEIGHT, 0 };
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(s->camera);
    rlDisableDepthMask(); // for transparency
    BeginBlendMode(BLEND_ALPHA);
    if (s->p1->alpha > PADDLE_MIN_ALPHA) {
        DrawCubeV(s->p1->pos, paddle_size, Fade(PADDLE_1_COLOR, s->p1->alpha));
        s->p1->alpha -= PADDLE_FADE_SPEED;
        if (s->p1->alpha < PADDLE_MIN_ALPHA) { s->p1->alpha = PADDLE_MIN_ALPHA; }
    }
    if (s->p2->alpha > PADDLE_MIN_ALPHA) {
        DrawCubeV(s->p2->pos, paddle_size, Fade(PADDLE_2_COLOR, s->p2->alpha));
        s->p2->alpha -= PADDLE_FADE_SPEED;
        if (s->p2->alpha < PADDLE_MIN_ALPHA) { s->p2->alpha = PADDLE_MIN_ALPHA; }
    }
    DrawCubeWiresV(s->p1->pos, paddle_size, PADDLE_1_COLOR);
    DrawCubeWiresV(s->p2->pos, paddle_size, PADDLE_2_COLOR);
    DrawSphereWires(s->ball, BALL_RADIUS, BALL_DETAIL, BALL_DETAIL, BALL_COLOR);
    float tracker_z = s->ball.z * (ARENA_LENGTH / (float)(ARENA_LENGTH - BALL_RADIUS * 2));
    Vector3 tracker_pos = { 0, 0, tracker_z };
    Vector3 tracker_size = { ARENA_WIDTH, ARENA_HEIGHT, 0 };
    DrawCubeWiresV(tracker_pos, tracker_size, BALL_COLOR);
    float slice_length = ARENA_LENGTH / (float)ARENA_SLICES;
    Vector3 slice_pos = { 0, 0, PADDLE_SPACING - slice_length/2.0 };
    Vector3 slice_size = { ARENA_WIDTH, ARENA_HEIGHT, slice_length };
    for (int i = 0; i < 8; i++) {
        DrawCubeWiresV(slice_pos, slice_size, ARENA_COLOR);
        slice_pos.z -= slice_length;
    }
    EndMode3D();
    EndDrawing();
}

static void _loop(void *arg) {
    CState *s = (CState*)arg;
    _update(s);
    _draw(s);
}

static void _destroy(CState *s){
    c_state_destroy(s);
    CloseAudioDevice();
    CloseWindow();
}

int main() {
    InitWindow(1, 1, "3D Pong");
    float width = GetMonitorWidth(GetCurrentMonitor()) * 0.8;
    float height = width * 0.7;
    SetWindowSize(width, height);
    InitAudioDevice();
    CState *s = c_state_create();
    EMSCRIPTEN_WEBSOCKET_T ws = c_ws_init(s);
    s->socket = ws;
    if (ws == -1) {
        return 0;
    }
    emscripten_set_main_loop_arg(_loop, s, FPS, 1);
    _destroy(s);
    return 0;
}
