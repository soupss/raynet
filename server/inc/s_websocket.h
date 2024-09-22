#ifndef S_WEBSOCKET_H
#define S_WEBSOCKET_H

#include <libwebsockets.h>

struct lws_context *s_ws_create_context();
void send_ball(SState* state);
#endif
