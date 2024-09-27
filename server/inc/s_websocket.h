#ifndef S_WEBSOCKET_H
#define S_WEBSOCKET_H

#include <libwebsockets.h>
#include <stdbool.h>
#include "s_state.h"
#include "shared_constants.h"

struct lws_context *s_ws_create_context();
void s_ws_send_ball_state(SState* state);
void s_ws_send_ball_hit_paddle(SState *s, PADDLE_SIDE side);
void s_ws_send_ball_hit_wall(SState *s);
void s_ws_send_ball_out_of_bounds(SState *s);
bool s_ws_two_paddles_connected(SState *state);

#endif
