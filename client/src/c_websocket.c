#include <emscripten/websocket.h>
#include <stdio.h>
#include "c_websocket.h"
#include "c_constants.h"
#include "shared_constants.h"


void c_ws_send_player_state(EMSCRIPTEN_WEBSOCKET_T ws, float pos[2]) {
    unsigned short ready_state = 0;
    emscripten_websocket_get_ready_state(ws, &ready_state);
    if (ready_state == WEBSOCKET_OPEN) {
        EMSCRIPTEN_RESULT result = emscripten_websocket_send_binary(ws, pos, 2 * sizeof(float));
        if (result != EMSCRIPTEN_RESULT_SUCCESS) {
            printf("Send state fail\n");
        }
    }
    else {
        printf("WebSocket not open\n");
    }
}

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
    CState *s = (CState *)user_data;
    MESSAGE_TYPE msg_type;
    memcpy(&msg_type, e->data, sizeof(MESSAGE_TYPE));
    unsigned char *payload = (unsigned char *)e->data + sizeof(MESSAGE_TYPE);
    memcpy(payload, e->data + sizeof(MESSAGE_TYPE), e->numBytes - sizeof(MESSAGE_TYPE));
    switch (msg_type) {
        case MSG_TYPE_SEND_PADDLE:
            {
                PLAYER_SIDE side;
                memcpy(&side, payload, sizeof(PLAYER_SIDE));
                float pos[2];
                memcpy(&pos, payload + sizeof(PLAYER_SIDE), 2 * sizeof(float));
                if (side == SIDE_1) {
                    s->player1.x = pos[0];
                    s->player1.y = pos[1];
                }
                if (side == SIDE_2) {
                    s->player2.x = pos[0];
                    s->player2.y = pos[1];
                }
            }
            break;
        case MSG_TYPE_SEND_BALL:
            memcpy(&s->ball, payload, 3*sizeof(float));
            break;
        case MSG_TYPE_ASSIGN_SIDE:
            {
                PLAYER_SIDE side;
                memcpy(&side, payload, sizeof(PLAYER_SIDE));
                if (side == SIDE_1) {
                    s->camera.position.z = CAMERA_DISTANCE;
                    s->side = SIDE_1;
                }
                else if (side == SIDE_2) {
                    s->camera.position.z = -CAMERA_DISTANCE;
                    s->side = SIDE_2;
                }
            }
            break;
        case MSG_TYPE_PLAYER_DISCONNECT:
            {
                PLAYER_SIDE side;
                memcpy(&side, payload, sizeof(PLAYER_SIDE));
                if (side == SIDE_1) {
                    s->player1.x = OUT_OF_BOUNDS;
                }
                else if (side == SIDE_2) {
                    s->player2.x = OUT_OF_BOUNDS;
                }
            }
            break;
        default:
            return EM_TRUE;
            break;
    }
}

EMSCRIPTEN_WEBSOCKET_T c_ws_init(CState *state) {
    if (!emscripten_websocket_is_supported()) {
        printf("WebSockets are not supported in this environment.\n");
        return -1;
    }
    EmscriptenWebSocketCreateAttributes attr = {
        "ws://" SERVER_URL ":9000",
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
