#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <emscripten/websocket.h>
#include "c_websocket.h"
#include "c_state.h"
#include "c_constants.h"
#include "shared_constants.h"

static void _c_update(CState *s) {
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
    c_ws_send_player_state(s->socket, pos);
    if (s->side == SIDE_1) {
        s->player1.x = pos[0];
        s->player1.y = pos[1];
    }
    else if (s->side == SIDE_2) {
        s->player2.x = pos[0];
        s->player2.y = pos[1];
    }
}

static void _c_draw(CState *s) {
    Vector3 paddle_size = { PADDLE_WIDTH, PADDLE_HEIGHT, 0 };
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(s->camera);
    DrawCubeWiresV(s->player1, paddle_size, GREEN);
    DrawCubeWiresV(s->player2, paddle_size, RED);
    DrawSphereWires(s->ball, BALL_RADIUS, BALL_DETAIL, BALL_DETAIL, WHITE);
    float tracker_z = s->ball.z * (ARENA_LENGTH / (float)(ARENA_LENGTH - BALL_RADIUS * 2));
    Vector3 tracker_pos = { 0, 0, tracker_z };
    Vector3 tracker_size = { ARENA_WIDTH, ARENA_HEIGHT, 0 };
    DrawCubeWiresV(tracker_pos, tracker_size, WHITE);
    float slice_length = ARENA_LENGTH / (float)ARENA_SLICES;
    Vector3 slice_pos = { 0, 0, PADDLE_SPACING - slice_length/2.0 };
    Vector3 slice_size = { ARENA_WIDTH, ARENA_HEIGHT, slice_length };
    for (int i = 0; i < 8; i++) {
        DrawCubeWiresV(slice_pos, slice_size, GREEN);
        slice_pos.z -= slice_length;
    }
    EndMode3D();
    EndDrawing();
}

static void _c_loop(void *arg) {
    CState *s = (CState*)arg;
    _c_update(s);
    _c_draw(s);
}

int main() {
    InitWindow(1, 1, "hlkjlkjj");
    float width = GetMonitorWidth(GetCurrentMonitor()) * 0.8;
    float height = width * 0.7;
    SetWindowSize(width, height);
    CState *s = c_state_create();
    EMSCRIPTEN_WEBSOCKET_T ws = c_ws_init(s);
    s->socket = ws;
    if (ws == -1) {
        return 0;
    }
    emscripten_set_main_loop_arg(_c_loop, s, FPS, 1);
    CloseWindow();
    return 0;
}
