#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "s_state.h"
#include "s_websocket.h"

static int interrupted = 0;

static void signal_handler() {
    interrupted = 1;
}

void service_loop(struct lws_context* context) {
    while(!interrupted) {
        lws_service(context, 1000);
    }
    lws_context_destroy(context);
}

void game_loop(SState * state, double dt) {
    state->ball->pos[0] += state->ball->speed[0]*dt;
    if (state->ball->pos[0] > 50 || state->ball->pos[0] < -50) {
        state->ball->pos[0] -= state->ball->speed[0]*dt;
        state->ball->speed[0] *= -1;
    }
}
#define TICK_RATE 120
int main() {
    signal(SIGINT, signal_handler);
    struct lws_context *context = s_ws_create_context();
    SState * state = lws_context_user(context);

    state->ball->speed[0] = 45;
    //Doesn't work, need to compile libwebsocket with flags that allow threadpooling
    //const struct lws_threadpool_create_args settings = {1,10};
    //lws_threadpool_create(context,&settings, "Thread %i");

    //Spawn thread that handles requests
    //Frees up main thread to handle game loop
    pthread_t * t = malloc(sizeof(pthread_t)); 
    pthread_create(t,NULL,(void * _Nullable (* _Nonnull)(void * _Nullable)) &service_loop, context);
    
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
        ///
        accumulated_time += dt;


        game_loop(state,dt);
        if (accumulated_time * TICK_RATE > 1) {
            send_ball(state);
            accumulated_time = 0;
        }
    }

    //Not correct way to kill a thread 
    //since the thread might be in the middle of doing something important
    pthread_cancel(t);
    return 0;
}
