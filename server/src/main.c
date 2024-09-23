#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <stdbool.h>
#include "s_state.h"
#include "s_websocket.h"
#include "s_constants.h"
#include "shared_constants.h"
#include "shared_functions.h"

static int interrupted = 0;

static void _s_signal_handler() {
    interrupted = 1;
}

static void _s_reset_ball(SState * state) {
    state->ball->pos[0] = 0;
    state->ball->pos[1] = 0;
    state->ball->pos[2] = 0;

    state->ball->vel[0] = 0;
    state->ball->vel[1] = 0;
    state->ball->vel[2] = BALL_STARTING_SPEED;

    state->ball->curve[0] = 0;
    state->ball->curve[1] = 0;
}

bool _s_paddle_hit_ball(SState * state) {
    SPaddle * p = state->ball->pos[2]>0 ? state->p1 : state->p2;
    float bx = state->ball->pos[0];
    float by = state->ball->pos[1];
    float px = p->pos[0];
    float py = p->pos[1];
    unsigned char is_horizontally_between_edges = bx > px - PADDLE_WIDTH /2.0 && bx < px + PADDLE_WIDTH/2.0;
    unsigned char is_horizontally_between_extended_edges = bx > px - PADDLE_WIDTH / 2.0 - BALL_RADIUS && bx < px + PADDLE_WIDTH/2.0 + BALL_RADIUS;
    unsigned char is_vertically_between_edges = by > py - PADDLE_HEIGHT/2.0 && by < py + PADDLE_HEIGHT/2.0;
    unsigned char is_vertically_between_extended_edges = by > py - PADDLE_HEIGHT/2.0 - BALL_RADIUS && by < py + PADDLE_HEIGHT/2.0 + BALL_RADIUS ;
    unsigned char case1 = is_horizontally_between_edges && is_vertically_between_extended_edges;
    unsigned char case2 = is_vertically_between_edges && is_horizontally_between_extended_edges;
    unsigned char case3 = shared_get_distance(bx,by,px + PADDLE_WIDTH/2.0,py + PADDLE_HEIGHT/2.0) < BALL_RADIUS;
    unsigned char case4 = shared_get_distance(bx,by,px - PADDLE_WIDTH/2.0,py + PADDLE_HEIGHT/2.0) < BALL_RADIUS;
    unsigned char case5 = shared_get_distance(bx,by,px + PADDLE_WIDTH/2.0,py - PADDLE_HEIGHT/2.0) < BALL_RADIUS;
    unsigned char case6 = shared_get_distance(bx,by,px - PADDLE_WIDTH/2.0,py - PADDLE_HEIGHT/2.0) < BALL_RADIUS;
    return case1 || case2 || case3 || case4 || case5 || case6 ;
}

static void _s_game_loop(SState *s, double dt) {
    if (s_ws_two_paddles_connected(s)) {
        s->ball->vel[0] += s->ball->curve[0] * BALL_CURVE_INCREASE_FACTOR * dt;
        s->ball->vel[1] += s->ball->curve[1] * BALL_CURVE_INCREASE_FACTOR * dt;
        s->ball->vel[2] += s->ball->vel[2] * BALL_ZVEL_INCREASE_FACTOR * dt;
        s->ball->pos[0] += s->ball->vel[0]*dt;
        bool left = s->ball->pos[0] + BALL_RADIUS > ARENA_WIDTH / 2.0;
        bool right = s->ball->pos[0] - BALL_RADIUS < -ARENA_WIDTH / 2.0;
        if (left || right) {
            s->ball->pos[0] -= s->ball->vel[0]*dt;
            s->ball->vel[0] *= -1;
        }
        s->ball->pos[1] += s->ball->vel[1]*dt;
        bool top = s->ball->pos[1] + BALL_RADIUS > ARENA_HEIGHT / 2.0;
        bool bottom = s->ball->pos[1] - BALL_RADIUS < -ARENA_HEIGHT / 2.0;
        if (top || bottom) {
            s->ball->pos[1] -= s->ball->vel[1]*dt;
            s->ball->vel[1] *= -1;
        }
        s->ball->pos[2] += s->ball->vel[2]*dt;
        bool paddle_1_side = s->ball->pos[2] + BALL_RADIUS > ARENA_LENGTH / 2.0;
        bool paddle_2_side = s->ball->pos[2] - BALL_RADIUS < -ARENA_LENGTH / 2.0;
        if (paddle_1_side) {
            if (_s_paddle_hit_ball(s)) {
                s->ball->pos[2] -= s->ball->vel[2]*dt;
                s->ball->vel[2] *= -1;
                float p1_vel[2] = {
                    (s->p1->pos_prev[0] - s->p1->pos[0]) / s->p1->pos_prev_dt,
                    (s->p1->pos_prev[1] - s->p1->pos[1]) / s->p1->pos_prev_dt,
                };
                printf("p1 curve (%f, %f)\n", p1_vel[0], p1_vel[1]);
                float curve[2] = {
                    s->ball->curve[0] - p1_vel[0] * BALL_CURVE_FACTOR,
                    s->ball->curve[1] - p1_vel[1] * BALL_CURVE_FACTOR
                };
                memcpy(&s->ball->curve, &curve, 2 * sizeof(float));
                s_ws_send_paddle_hit_ball(s, SIDE_1);
            }
            else {
                _s_reset_ball(s);
            }
        }
        else if (paddle_2_side) {
            if (_s_paddle_hit_ball(s)) {
                s->ball->pos[2] -= s->ball->vel[2]*dt;
                s->ball->vel[2] *= -1;
                float p2_vel[2] = {
                    (s->p2->pos_prev[0] - s->p2->pos[0]) / s->p2->pos_prev_dt,
                    (s->p2->pos_prev[1] - s->p2->pos[1]) / s->p2->pos_prev_dt,
                };
                printf("p2 vel (%f, %f)\n", p2_vel[0], p2_vel[1]);
                float curve[2] = {
                    s->ball->curve[0] + p2_vel[0] * BALL_CURVE_FACTOR,
                    s->ball->curve[1] + p2_vel[1] * BALL_CURVE_FACTOR
                };
                memcpy(&s->ball->curve, &curve, 2 * sizeof(float));
                s_ws_send_paddle_hit_ball(s, SIDE_2);
            }
            else {
                _s_reset_ball(s);
            }
        }
    }
}

static void _s_thread_service_loop(struct lws_context *context) {
    while(!interrupted) {
        lws_service(context, 1000);
    }
    lws_context_destroy(context);
}

int main() {
    srand(time(NULL));
    signal(SIGINT, _s_signal_handler);
    struct lws_context *context = s_ws_create_context();
    SState *state = lws_context_user(context);
    _s_reset_ball(state);

    pthread_t *t = malloc(sizeof(pthread_t));
    pthread_create(t, NULL, (void * _Nullable (* _Nonnull)(void * _Nullable)) &_s_thread_service_loop, context);

    struct timeval t1;
    gettimeofday(&t1, NULL);
    double accumulated_time = 0;
    while (!interrupted)
    {
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
