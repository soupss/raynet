#include <libwebsockets.h>
#include "s_websocket.h"
#include "shared_constants.h"

void s_ws_send_ball_state(SState *s) {
    int payload_size = sizeof(MESSAGE_TYPE) + 3 * sizeof(float);
    unsigned char buffer[LWS_PRE + payload_size];
    MESSAGE_TYPE m = MSG_TYPE_SEND_BALL;
    memcpy(&buffer[LWS_PRE], &m, sizeof(MESSAGE_TYPE));
    memcpy(&buffer[LWS_PRE + sizeof(MESSAGE_TYPE)], s->ball->pos, 3 * sizeof(float));
    if (s->p1->wsi != NULL) {
        lws_write(s->p1->wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
    }
    if (s->p2->wsi != NULL) {
        lws_write(s->p2->wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
    }
}

void s_ws_send_paddle_hit_ball(SState *s, PADDLE_SIDE side) {
    int payload_size = sizeof(MESSAGE_TYPE) + sizeof(PADDLE_SIDE);
    unsigned char buffer[LWS_PRE + payload_size];
    MESSAGE_TYPE m = MSG_TYPE_PADDLE_HIT_BALL;
    memcpy(&buffer[LWS_PRE], &m, sizeof(MESSAGE_TYPE));
    memcpy(&buffer[LWS_PRE + sizeof(MESSAGE_TYPE)], &side, sizeof(PADDLE_SIDE));
    if (s->p1->wsi != NULL) {
        lws_write(s->p1->wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
    }
    if (s->p2->wsi != NULL) {
        lws_write(s->p2->wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
    }
}

bool s_ws_two_paddles_connected(SState *s) {
    return (s->p1->wsi != NULL) && (s->p2->wsi != NULL);
}

//TODO: make function that creates message
static void _s_ws_send_paddle_position(SState *s, float *pos, PADDLE_SIDE side) {
    bool p1_connected = s->p1->wsi != NULL;
    bool p2_connected = s->p2->wsi != NULL;
    if (side == SIDE_1 && !p1_connected) { return; }
    if (side == SIDE_2 && !p2_connected) { return; }
    int payload_size = sizeof(MESSAGE_TYPE) + sizeof(PADDLE_SIDE) + 2 * sizeof(float);
    unsigned char buffer[LWS_PRE + payload_size];
    MESSAGE_TYPE m = MSG_TYPE_SEND_PADDLE;
    memcpy(&buffer[LWS_PRE], &m, sizeof(MESSAGE_TYPE));
    memcpy(&buffer[LWS_PRE + sizeof(MESSAGE_TYPE)], &side, sizeof(PADDLE_SIDE));
    memcpy(&buffer[LWS_PRE + sizeof(MESSAGE_TYPE) + sizeof(PADDLE_SIDE)], pos, 2 * sizeof(float));
    if (p1_connected) {
        lws_write(s->p1->wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
    }
    if (p2_connected) {
        lws_write(s->p2->wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
    }
}

// change name to broadcast ?
static void _s_ws_send_paddle_positions(SState * s) {
    _s_ws_send_paddle_position(s, s->p1->pos, SIDE_1);
    _s_ws_send_paddle_position(s, s->p2->pos, SIDE_2);
}

static void _s_ws_send_assign_side(struct lws *recipient_wsi, PADDLE_SIDE side) {
    int payload_size = sizeof(MESSAGE_TYPE) + sizeof(PADDLE_SIDE);
    unsigned char buffer[LWS_PRE + payload_size];
    MESSAGE_TYPE m = MSG_TYPE_ASSIGN_SIDE;
    memcpy(&buffer[LWS_PRE], &m, sizeof(MESSAGE_TYPE));
    memcpy(&buffer[LWS_PRE + sizeof(MESSAGE_TYPE)], &side, sizeof(PADDLE_SIDE));
    lws_write(recipient_wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
}

static void _s_ws_send_paddle_disconnect(SState *state, struct lws *recipient_wsi, PADDLE_SIDE dc_side) {
    int payload_size = sizeof(MESSAGE_TYPE) + sizeof(PADDLE_SIDE);
    unsigned char buffer[LWS_PRE + payload_size];
    MESSAGE_TYPE m = MSG_TYPE_PADDLE_DISCONNECT;
    memcpy(&buffer[LWS_PRE], &m, sizeof(MESSAGE_TYPE));
    memcpy(&buffer[LWS_PRE + sizeof(MESSAGE_TYPE)], &dc_side, sizeof(PADDLE_SIDE));
    lws_write(recipient_wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
}

static int _s_ws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    SState *state = lws_context_user(lws_get_context(wsi));
    switch(reason) {
        case LWS_CALLBACK_ESTABLISHED:
            if (state->p1->wsi == NULL) {
                state->p1->wsi = wsi;
                _s_ws_send_assign_side(wsi, SIDE_1);
                printf("paddle 1 connect\n");
            }
            else if (state->p2->wsi == NULL) {
                state->p2->wsi = wsi;
                _s_ws_send_assign_side(wsi, SIDE_2);
                printf("paddle 2 connect\n");
            }
            else {
                // TODO:spectators
                printf("Game full");
                return -1; // close connection
            }
            break;
        case LWS_CALLBACK_CLOSED:
            if (wsi == state->p1->wsi) {
                state->p1->wsi = NULL;
                printf("paddle 1 disconnect\n");
                _s_ws_send_paddle_disconnect(state, state->p2->wsi, SIDE_1);
            }
            else if (wsi == state->p2->wsi) {
                state->p2->wsi = NULL;
                printf("paddle 2 disconnect\n");
                _s_ws_send_paddle_disconnect(state, state->p1->wsi, SIDE_2);
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
    info.port = PORT;
    info.protocols = _s_ws_protocols;
    info.user = s_state_create();
    struct lws_context *context = lws_create_context(&info);
    if(!context) {
        lwsl_err("Failed to create WebSocket context\n");
        return 0;
    }
    return context;
}
