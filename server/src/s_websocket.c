#include <libwebsockets.h>
#include <time.h>
#include "s_websocket.h"
#include "s_constants.h"
#include "shared_constants.h"
#include "queue.h"

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

static void _send_paddle_position(SState *s, float *pos, PADDLE_SIDE side) {
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

static void _send_paddle_positions(SState * s) {
    _send_paddle_position(s, s->p1->pos, SIDE_1);
    _send_paddle_position(s, s->p2->pos, SIDE_2);
}

static void _send_assign_side(struct lws *recipient_wsi, PADDLE_SIDE side) {
    int payload_size = sizeof(MESSAGE_TYPE) + sizeof(PADDLE_SIDE);
    unsigned char buffer[LWS_PRE + payload_size];
    MESSAGE_TYPE m = MSG_TYPE_ASSIGN_SIDE;
    memcpy(&buffer[LWS_PRE], &m, sizeof(MESSAGE_TYPE));
    memcpy(&buffer[LWS_PRE + sizeof(MESSAGE_TYPE)], &side, sizeof(PADDLE_SIDE));
    lws_write(recipient_wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
}

static void _send_paddle_disconnect(struct lws *recipient_wsi, PADDLE_SIDE dc_side) {
    if (recipient_wsi == NULL) {return;}
    int payload_size = sizeof(MESSAGE_TYPE) + sizeof(PADDLE_SIDE);
    unsigned char buffer[LWS_PRE + payload_size];
    MESSAGE_TYPE m = MSG_TYPE_PADDLE_DISCONNECT;
    memcpy(&buffer[LWS_PRE], &m, sizeof(MESSAGE_TYPE));
    memcpy(&buffer[LWS_PRE + sizeof(MESSAGE_TYPE)], &dc_side, sizeof(PADDLE_SIDE));
    lws_write(recipient_wsi, &buffer[LWS_PRE], payload_size, LWS_WRITE_BINARY);
}

static int _callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    SState *s = lws_context_user(lws_get_context(wsi));
    switch(reason) {
        case LWS_CALLBACK_ESTABLISHED:
            if (s->p1->wsi == NULL) {
                s->p1->wsi = wsi;
                _send_assign_side(wsi, SIDE_1);
                printf("paddle 1 connect\n");
            }
            else if (s->p2->wsi == NULL) {
                s->p2->wsi = wsi;
                _send_assign_side(wsi, SIDE_2);
                printf("paddle 2 connect\n");
            }
            else {
                // TODO:spectators
                printf("Game full");
                return -1; // close connection
            }
            break;

        case LWS_CALLBACK_CLOSED:
            if (wsi == s->p1->wsi) {
                s->p1->wsi = NULL;
                _send_paddle_disconnect(s->p2->wsi, SIDE_1);
                printf("paddle 1 disconnect\n");
            }
            else if (wsi == s->p2->wsi) {
                s->p2->wsi = NULL;
                _send_paddle_disconnect(s->p1->wsi, SIDE_2);
                printf("paddle 2 disconnect\n");
            }
            break;

        case LWS_CALLBACK_RECEIVE:
            {
                if (wsi == s->p1->wsi) {
                    float *p1_pos = malloc(2 * sizeof(float));
                    memcpy(p1_pos, in, 2 * sizeof(float));
                    queue_add(s->p1->pos_history, p1_pos);
                    struct timeval tv;
                    gettimeofday(&tv, NULL);
                    long long t = (long long)tv.tv_sec * 1000000 + tv.tv_usec;
                    static long long _p1_pos_prev_t = 0;
                    if (_p1_pos_prev_t != 0) {
                        float *dt_pointer = malloc(sizeof(float));
                        *dt_pointer = t - _p1_pos_prev_t;
                        queue_add(s->p1->pos_dt_history, dt_pointer);
                    }
                    _p1_pos_prev_t = t;
                    if (s->p1->pos_history->length >= PADDLE_POS_HISTORY_LENGTH) {
                        queue_remove(s->p1->pos_history);
                    }
                    if (s->p1->pos_dt_history->length >= PADDLE_POS_HISTORY_LENGTH - 1) {
                        queue_remove(s->p1->pos_dt_history);
                    }
                    memcpy(s->p1->pos, p1_pos, 2 * sizeof(float));
                    _send_paddle_positions(s);
                }
                else if (wsi == s->p2->wsi) {
                    float *p2_pos = malloc(2 * sizeof(float));
                    memcpy(p2_pos, in, 2 * sizeof(float));
                    queue_add(s->p2->pos_history, p2_pos);
                    struct timeval tv;
                    gettimeofday(&tv, NULL);
                    long long t = (long long)tv.tv_sec * 1000000 + tv.tv_usec;
                    static long long _p2_pos_prev_t = 0;
                    if (_p2_pos_prev_t != 0) {
                        float *dt_pointer = malloc(sizeof(float));
                        *dt_pointer = t - _p2_pos_prev_t;
                        queue_add(s->p2->pos_dt_history, dt_pointer);
                    }
                    _p2_pos_prev_t = t;
                    if (s->p2->pos_history->length >= PADDLE_POS_HISTORY_LENGTH) {
                        queue_remove(s->p2->pos_history);
                    }
                    if (s->p2->pos_dt_history->length >= PADDLE_POS_HISTORY_LENGTH - 2) {
                        queue_remove(s->p2->pos_dt_history);
                    }
                    memcpy(s->p2->pos, p2_pos, 2 * sizeof(float));
                    _send_paddle_positions(s);
                }
            }
            break;
        default:
            break;
    }
    return 0;
}

static struct lws_protocols _protocols[] = {
    {
        "client-server",
        _callback,
        0, // per session data size
        1024, // max frame size
    },
    { NULL, NULL, 0, 0 } // null terminate
};

struct lws_context *s_ws_create_context() {
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = PORT;
    info.protocols = _protocols;
    info.user = s_state_create();
    struct lws_context *context = lws_create_context(&info);
    if(!context) {
        lwsl_err("Failed to create WebSocket context\n");
        return 0;
    }
    return context;
}
