#include <emscripten/websocket.h>
#include <stdio.h>
#include "c_websocket.h"

static EM_BOOL _on_open(int event_type, const EmscriptenWebSocketOpenEvent *e, void *user_data) {
    printf("Websocket open\n");
    return EM_TRUE;
}

static EM_BOOL _on_close(int event_type, const EmscriptenWebSocketCloseEvent *e, void *user_data) {
    printf("Websocket closed\n");
    return EM_TRUE;
}

static EM_BOOL _on_error(int event_type, const EmscriptenWebSocketErrorEvent *e, void *user_data) {
    printf("Websocket error\n");
    return EM_TRUE;
}

static EM_BOOL _on_message(int event_type, const EmscriptenWebSocketMessageEvent *e, void *user_data) {
    CState *state = (CState *)user_data;
    float enemy_pos[2] = {0};
    memcpy(enemy_pos, e->data, 2 * sizeof(float));
    state->enemy.x = -enemy_pos[0];
    state->enemy.y = enemy_pos[1];
    return EM_TRUE;
}

#define URL "localhost"
EMSCRIPTEN_WEBSOCKET_T c_ws_init(CState *state) {
    if (!emscripten_websocket_is_supported()) {
        printf("WebSockets are not supported in this environment.\n");
        return -1;
    }
    EmscriptenWebSocketCreateAttributes attr = {
        "ws://" URL ":9000",
        NULL,
        EM_TRUE
    };
    EMSCRIPTEN_WEBSOCKET_T ws = emscripten_websocket_new(&attr);
    if (ws <= 0) {
        printf("Failed to create websocket");
        return -1;
    }
    emscripten_websocket_set_onopen_callback(ws, state, _on_open);
    emscripten_websocket_set_onerror_callback(ws, state, _on_error);
    emscripten_websocket_set_onclose_callback(ws, state, _on_close);
    emscripten_websocket_set_onmessage_callback(ws, state, _on_message);
    return ws;
}

#define READY 1
void c_ws_send_player_state(EMSCRIPTEN_WEBSOCKET_T ws, float pos[2]) {
    unsigned short ready_state = 0;
    emscripten_websocket_get_ready_state(ws, &ready_state);
    if (ready_state == READY) {
        EMSCRIPTEN_RESULT result = emscripten_websocket_send_binary(ws, pos, 2 * sizeof(float));
        if (result != EMSCRIPTEN_RESULT_SUCCESS) {
            printf("Send state fail\n");
        }
    }
    else {
        printf("WebSocket is not open, unable to send message\n");
    }
}
