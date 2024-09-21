#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <emscripten/websocket.h>
#include "c_websocket.h"
#include "c_state.h"

static void _c_update(CState *s) {
    Ray mouse = GetMouseRay(GetMousePosition(), s->camera);
    float d = (PADDLE_SPACING - mouse.position.z) / mouse.direction.z; // distance from camera to paddle
    float pos[2] = {mouse.position.x + mouse.direction.x * d,
                    mouse.position.y + mouse.direction.y * d};
    c_ws_send_player_state(s->socket, pos);
    s->player.x = pos[0];
    s->player.y = pos[1];
}

static void _c_draw(CState *s) {
    Vector3 paddle_size = { PADDLE_SIZE, PADDLE_SIZE, 0 };
    BeginDrawing();
    ClearBackground(GRAY);
    BeginMode3D(s->camera);
    DrawCubeWiresV(s->player, paddle_size, GREEN);
    DrawCubeWiresV(s->enemy, paddle_size, GREEN);
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
    float width = GetMonitorWidth(GetCurrentMonitor()) * 0.6;
    float height = width * 0.6;
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
