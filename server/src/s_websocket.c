#include <libwebsockets.h>
#include "s_state.h"

#define SEND_PADDLE 1
#define SEND_BALL 2

void send_ball(SState * state) {
    if (state->p1->wsi == NULL && state->p2->wsi == NULL) {return;}

    int payload_size = 3 * sizeof(float);
    
    unsigned char buffer[LWS_PRE + 1 + payload_size];
    buffer[LWS_PRE] = SEND_BALL;

    memcpy(&buffer[LWS_PRE + 1], state->ball->pos, payload_size);
    if (state->p1->wsi != NULL) {
        lws_write(state->p1->wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
    }
    if (state->p2->wsi != NULL) {
        lws_write(state->p2->wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
    }
}

static void _s_ws_send_player_state(struct lws *recipient_wsi, float pos[2]) {
    if (recipient_wsi == NULL) {
        return;
    }
    int payload_size = 2 * sizeof(float);
    unsigned char buffer[LWS_PRE + 1 + payload_size];
    buffer[LWS_PRE] = SEND_PADDLE;
    memcpy(&buffer[LWS_PRE + 1], pos, payload_size);
    lws_write(recipient_wsi, &buffer[LWS_PRE], payload_size + 1, LWS_WRITE_BINARY);
}

static int _s_ws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    SState *s = lws_context_user(lws_get_context(wsi));
    switch(reason) {
        case LWS_CALLBACK_ESTABLISHED:
            if (s->p1->wsi == NULL) {
                s->p1->wsi = wsi;
                printf("Player 1 connect\n");
            }
            else if (s->p2->wsi == NULL) {
                s->p2->wsi = wsi;
                printf("Player 2 connect\n");
            }
            else {
                printf("Game full");
                return -1; // close connection
            }
            break;
        case LWS_CALLBACK_CLOSED:
            if (wsi == s->p1->wsi) {
                s->p1->wsi = NULL;
                printf("Player 1 disconnect\n");
            }
            else if (wsi == s->p2->wsi) {
                s->p2->wsi = NULL;
                printf("Player 2 disconnect\n");
            }
            break;
        case LWS_CALLBACK_RECEIVE:
            if (wsi == s->p1->wsi) {
                memcpy(s->p1->pos, in, 2 * sizeof(float));
                _s_ws_send_player_state(s->p2->wsi, s->p1->pos);
            }
            else if (wsi == s->p2->wsi) {
                memcpy(s->p2->pos, in, 2 * sizeof(float));
                _s_ws_send_player_state(s->p1->wsi, s->p2->pos);
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
