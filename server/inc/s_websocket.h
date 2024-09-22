#ifndef S_WEBSOCKET_H
#define S_WEBSOCKET_H

#include <libwebsockets.h>
#include "s_state.h"

struct lws_context *s_ws_create_context();
void s_ws_send_ball_state(SState* state);

#endif
