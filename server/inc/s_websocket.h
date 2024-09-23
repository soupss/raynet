#ifndef S_WEBSOCKET_H
#define S_WEBSOCKET_H

#include <libwebsockets.h>
#include <stdbool.h>
#include "s_state.h"
#include "shared_constants.h"

struct lws_context *s_ws_create_context();
void s_ws_send_ball_state(SState* state);
void s_ws_send_paddle_hit_ball(SState *s, PLAYER_SIDE side);
bool s_ws_two_players_connected(SState *state);

#endif
