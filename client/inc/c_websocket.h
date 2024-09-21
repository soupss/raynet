#ifndef C_WEBSOCKET_H
#define C_WEBSOCKET_H

#include <emscripten/websocket.h>
#include "c_state.h"

EMSCRIPTEN_WEBSOCKET_T c_ws_init(CState *state);
void c_ws_send_player_state(EMSCRIPTEN_WEBSOCKET_T ws, float pos[2]);

#endif
