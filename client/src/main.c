#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <emscripten/websocket.h>
#include "websocket.h"
#include "state.h"

static void _update(State *s) {
    Ray mouse = GetMouseRay(GetMousePosition(), s->camera);
    float d = (PADDLE_SPACING - mouse.position.z) / mouse.direction.z; // distance from camera to paddle
    float pos[2] = {mouse.position.x + mouse.direction.x * d,
                    mouse.position.y + mouse.direction.y * d};
    ws_send_player_state(s->socket, pos);
    s->player.x = pos[0];
    s->player.y = pos[1];
}

static void _draw(State *s) {
    Vector3 paddle_size = { PADDLE_SIZE, PADDLE_SIZE, 0 };
    BeginDrawing();
    ClearBackground(GRAY);
    BeginMode3D(s->camera);
    DrawCubeWiresV(s->player, paddle_size, GREEN);
    DrawCubeWiresV(s->enemy, paddle_size, GREEN);
    EndMode3D();
    EndDrawing();
}

static void _loop(void *arg) {
    State *s = (State*)arg;
    _update(s);
    _draw(s);
}

int main() {
    InitWindow(1, 1, "hlkjlkjj");
    float width = GetMonitorWidth(GetCurrentMonitor()) * 0.6;
    float height = width * 0.6;
    SetWindowSize(width, height);
    State *s = state_create();
    EMSCRIPTEN_WEBSOCKET_T ws = ws_init(s);
    s->socket = ws;
    if (ws == -1) {
        return 0;
    }
    emscripten_set_main_loop_arg(_loop, s, FPS, 1);
    CloseWindow();
    return 0;
}
