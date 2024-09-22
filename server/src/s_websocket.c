#include <libwebsockets.h>
#include "s_websocket.h"
#include "shared_constants.h"

void s_ws_send_ball_state(SState * state) {
    if (state->p1->wsi == NULL && state->p2->wsi == NULL) {return;}

    int payload_size = 3 * sizeof(float);
    unsigned char buffer[LWS_PRE + 1 + payload_size];
    buffer[LWS_PRE] = SEND_BALL;

    memcpy(&buffer[LWS_PRE + 1], state->ball->pos, payload_size);
    if (state->p1->wsi != NULL) {
        lws_write(state->p1->wsi, &buffer[LWS_PRE], 1 + payload_size, LWS_WRITE_BINARY);
    }
    if (state->p2->wsi != NULL) {
        lws_write(state->p2->wsi, &buffer[LWS_PRE], 1 + payload_size, LWS_WRITE_BINARY);
    }
}

static void _s_ws_send_paddle_position(SState * state,float * pos, unsigned char player) {
    int payload_size = 2 * sizeof(float);
    unsigned char buffer[LWS_PRE + 1 + payload_size];
    buffer[LWS_PRE] = player;
    memcpy(&buffer[LWS_PRE + 1], pos, payload_size);

    if (state->p1->wsi != NULL) {
        lws_write(state->p1->wsi, &buffer[LWS_PRE], payload_size + 1, LWS_WRITE_BINARY);
    }
    if (state->p2->wsi != NULL) {
        lws_write(state->p2->wsi, &buffer[LWS_PRE], payload_size + 1, LWS_WRITE_BINARY);
    }
}

static void _s_ws_send_paddle_positions(SState * state) {
    if (state->p1->wsi == NULL && state->p2->wsi == NULL) {return;}

    _s_ws_send_paddle_position(state, state->p1->pos, SEND_PADDLE_PLAYER_1);
    _s_ws_send_paddle_position(state, state->p2->pos, SEND_PADDLE_PLAYER_2);
}


static void s_ws_send_player_role(struct lws *recipient_wsi, unsigned char role) {
    unsigned char buffer[LWS_PRE + 1];
    buffer[LWS_PRE] = role;
    lws_write(recipient_wsi, &buffer[LWS_PRE], 1, LWS_WRITE_BINARY);
}

static int _s_ws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    SState *state = lws_context_user(lws_get_context(wsi));
    switch(reason) {
        case LWS_CALLBACK_ESTABLISHED:
            if (state->p1->wsi == NULL) {
                state->p1->wsi = wsi;
                s_ws_send_player_role(wsi,SEND_ROLE_1);
                printf("Player 1 connect\n");
            }
            else if (state->p2->wsi == NULL) {
                state->p2->wsi = wsi;
                s_ws_send_player_role(wsi,SEND_ROLE_2);
                printf("Player 2 connect\n");
            }
            else {
                printf("Game full");
                return -1; // close connection
            }
            break;
        case LWS_CALLBACK_CLOSED:
            if (wsi == state->p1->wsi) {
                state->p1->wsi = NULL;
                printf("Player 1 disconnect\n");
            }
            else if (wsi == state->p2->wsi) {
                state->p2->wsi = NULL;
                printf("Player 2 disconnect\n");
            }
            break;
        case LWS_CALLBACK_RECEIVE:
            if (wsi == state->p1->wsi) {
                memcpy(state->p1->pos, in, 2 * sizeof(float));
                _s_ws_send_paddle_positions(state);
            }
            else if (wsi == state->p2->wsi) {
                memcpy(state->p2->pos, in, 2 * sizeof(float));
                _s_ws_send_paddle_positions(state);
            }
            break;
        default:
            break;
    }
    return 0;
}

static struct lws_protocols _s_ws_protocols[] = {
    {
        "client-server",
        _s_ws_callback,
        0, // per session data size
        1024, // max frame size
    },
    { NULL, NULL, 0, 0 } // null terminate
};

struct lws_context *s_ws_create_context() {
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = 9000;
    info.protocols = _s_ws_protocols;
    info.user = s_state_create();
    struct lws_context *context = lws_create_context(&info);
    if(!context) {
        lwsl_err("Failed to create WebSocket context\n");
        return 0;
    }
    return context;
}
