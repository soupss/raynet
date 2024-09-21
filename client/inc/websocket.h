#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <emscripten/websocket.h>
#include "state.h"

EMSCRIPTEN_WEBSOCKET_T ws_init(State *state);
void ws_send_player_state(EMSCRIPTEN_WEBSOCKET_T ws, float pos[2]);

#endif
