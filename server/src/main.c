#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
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

static void _s_game_loop(SState *state, double dt) {
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
        state->ball->pos[2] -= state->ball->speed[2]*dt;
        state->ball->speed[2] *= -1;
    }
}

int main() {
    signal(SIGINT, _s_signal_handler);
    struct lws_context *context = s_ws_create_context();
    SState *state = lws_context_user(context);

    state->ball->speed[0] = 45;
    state->ball->speed[1] = 100;
    state->ball->speed[2] = 150;

    //Spawn thread that handles requests
    //Frees up main thread to handle game loop
    pthread_t *t = malloc(sizeof(pthread_t));
    pthread_create(t, NULL, &_s_thread_service_loop, context);

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
        accumulated_time += dt;
        _s_game_loop(state, dt);
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
