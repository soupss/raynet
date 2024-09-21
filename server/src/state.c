#include <libwebsockets.h>
#include "state.h"

State *state_create() {
    State *s = malloc(sizeof(State));
    s->p1 = malloc(sizeof(Player));
    s->p2 = malloc(sizeof(Player));
    float pos[2] = {0, 0};
    memcpy(s->p1->pos, pos, sizeof(pos));
    memcpy(s->p2->pos, pos, sizeof(pos));
    s->p1->wsi = NULL;
    s->p2->wsi = NULL;
    return s;
}
