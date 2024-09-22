#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "s_state.h"
#include "s_websocket.h"
#include "s_constants.h"
#include "shared_constants.h"

static int interrupted = 0;

static void _s_signal_handler() {
    interrupted = 1;
}

static void _s_thread_service_loop(struct lws_context *context) {
    while(!interrupted) {
        lws_service(context, 1000);
    }
    lws_context_destroy(context);
}

static float distance(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)); 
}

static void _s_reset_ball(SState * state) {
    state->ball->pos[0] = 0;
    state->ball->pos[1] = 0;
    state->ball->pos[2] = 0;

    state->ball->speed[0] = 13;
    state->ball->speed[1] = 15;
    state->ball->speed[2] = 41;
}

static unsigned char _s_ball_hits_paddle(SState * state) {
    SPlayer * p = state->ball->pos[2]>0 ? state->p1 : state->p2;
    float bx = state->ball->pos[0];
    float by = state->ball->pos[1];
    float px = p->pos[0];
    float py = p->pos[1];

    unsigned char is_horizontally_between_edges = bx > px - PADDLE_WIDTH/2 && bx < px + PADDLE_WIDTH/2;
    unsigned char is_horizontally_between_extended_edges = bx > px - PADDLE_WIDTH/2 - BALL_RADIUS && bx < px + PADDLE_WIDTH/2 + BALL_RADIUS;
    unsigned char is_vertically_between_edges = by > py - PADDLE_HEIGHT/2 && by < py + PADDLE_HEIGHT/2;
    unsigned char is_vertically_between_extended_edges = by > py - PADDLE_HEIGHT/2 - BALL_RADIUS && by < py + PADDLE_HEIGHT/2 + BALL_RADIUS ;

    unsigned char case1 = is_horizontally_between_edges && is_vertically_between_extended_edges;
    unsigned char case2 = is_vertically_between_edges && is_horizontally_between_extended_edges;
    unsigned char case3 = distance(bx,by,px + PADDLE_WIDTH/2,py + PADDLE_HEIGHT/2) < BALL_RADIUS;
    unsigned char case4 = distance(bx,by,px - PADDLE_WIDTH/2,py + PADDLE_HEIGHT/2) < BALL_RADIUS;
    unsigned char case5 = distance(bx,by,px + PADDLE_WIDTH/2,py - PADDLE_HEIGHT/2) < BALL_RADIUS;
    unsigned char case6 = distance(bx,by,px - PADDLE_WIDTH/2,py - PADDLE_HEIGHT/2) < BALL_RADIUS;

    return case1 || case2 || case3 || case4 || case5 || case6 ;
}

static void _s_game_loop(SState *state, double dt) {
    state->ball->speed[0] += state->ball->speed[0] * 0.125*dt;
    state->ball->speed[1] += state->ball->speed[1] * 0.125*dt;
    state->ball->speed[2] += state->ball->speed[2] * 0.125*dt;

    state->ball->pos[0] += state->ball->speed[0]*dt;
    if (state->ball->pos[0] > ARENA_WIDTH / 2.0 || state->ball->pos[0] < -ARENA_WIDTH / 2.0) {
        state->ball->pos[0] -= state->ball->speed[0]*dt;
        state->ball->speed[0] *= -1;
    }
    state->ball->pos[1] += state->ball->speed[1]*dt;
    if (state->ball->pos[1] > ARENA_HEIGHT / 2.0 || state->ball->pos[1] < -ARENA_HEIGHT / 2.0) {
        state->ball->pos[1] -= state->ball->speed[1]*dt;
        state->ball->speed[1] *= -1;
    }
    state->ball->pos[2] += state->ball->speed[2]*dt;
    if (state->ball->pos[2] > ARENA_LENGTH / 2.0 || state->ball->pos[2] < -ARENA_LENGTH / 2.0) {
            printf("FLIPPING! \n");
        if (_s_ball_hits_paddle(state)) {
            state->ball->pos[2] -= state->ball->speed[2]*dt;
            state->ball->speed[2] *= -1;
            printf("HIT! \n");
        }
        else {
            _s_reset_ball(state);
        }

    }
}

int main() {
    signal(SIGINT, _s_signal_handler);
    struct lws_context *context = s_ws_create_context();
    SState *state = lws_context_user(context);
    _s_reset_ball(state);


    //Spawn thread that handles requests
    //Frees up main thread to handle game loop
    pthread_t *t = malloc(sizeof(pthread_t));
    pthread_create(t, NULL, (void * _Nullable (* _Nonnull)(void * _Nullable)) &_s_thread_service_loop, context);

    struct timeval t1;
    gettimeofday(&t1, NULL);
    double accumulated_time = 0;
    while (!interrupted)
    {
        /// Calculate delta time
        time_t previous_sec = t1.tv_sec;
        suseconds_t previous_usec = t1.tv_usec;
        gettimeofday(&t1, NULL);
        double dt = (t1.tv_sec - previous_sec) * 1000.0;
        dt += (t1.tv_usec - previous_usec) / 1000.0;
        dt /= 1000;
        _s_game_loop(state, dt);

        accumulated_time += dt;
        if (accumulated_time * TICK_RATE > 1) {
            s_ws_send_ball_state(state);
            accumulated_time = 0;
        }
    }

    //Not correct way to kill a thread
    //since the thread might be in the middle of doing something important
    pthread_cancel(t);
    return 0;
}
