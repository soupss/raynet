#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static int interrupted = 0;

static void signal_handler() {
    interrupted = 1;
}

typedef struct State {
    double pos[3];
    int cube_dir;
    struct lws *player1, *player2;
} State ;

static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason,
        void *user, void *in, size_t len) {
    switch(reason) {
        case LWS_CALLBACK_CLOSED:;
            {struct lws_context * lws_cx = lws_get_context(wsi);
            State*  state = (State * ) (lws_context_user(lws_cx));
            if (*((int *) user) == 1) {
                printf("player1 left");
                state->player1 = 0;
            }
            else if (*((int *) user) == 2) {
                printf("player2 left");
                state->player2 = 0;
            }}
            break;
        case LWS_CALLBACK_ESTABLISHED:;
            struct lws_context * lws_cx = lws_get_context(wsi);
            State*  state = (State * ) (lws_context_user(lws_cx));
            
            if (state->player1 == 0) {
                printf("player1 added");
                state->player1 = wsi;
                *((int *) user) = 1;
            }
            else if (state->player2 == 0){
                printf("player2 added");
                state->player2 = wsi;
                *((int *) user) = 2;
            }
            else {
                printf("no4u");
                lws_set_timeout(wsi, PENDING_TIMEOUT_HOLDING_AH, LWS_TO_KILL_SYNC);
            }
            
            printf("\n");
            break;
        case LWS_CALLBACK_RECEIVE:
            for (int i = 0; i < len; i++)
            {
               printf("%c",((char *) in)[i]);
            }
            printf("\n");
            
            break;
        default:
            printf ("Reason: %d \n", (int) reason);
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "echo-protocol",
        callback_echo,
        1, // per session data size
        1024, // max frame size,
        1
    },
    { NULL, NULL, 0, 0 } // null terminate
};

State * initState() {
    State * s =(State *) malloc(sizeof(double)*3 + sizeof(int) + sizeof(struct lws *));
    s->pos[0] = 0;
    s->pos[1] = 0;
    s->pos[2] = 0;
    s->cube_dir = 1;
    s->player1 = 0;
    s->player2 = 0;

    return s;
}
void moveSquare(State* s, double dt) {
    printf("DeltaTime: %f \n", dt);
    printf("Before %f \n", s->pos[1]);
    s->pos[1] += s->cube_dir * 0.1 * dt;
    if (s->pos[1] > 5 || s->pos[1] < -5) {
        s->cube_dir *= -1;
    }
    printf("After: %f \n", s->pos[1]);
}

void sendState(State* state) {
    if (state->player1 != 0) {
        lws_write(state->player1, state->pos, sizeof(double)*3, LWS_WRITE_BINARY);
    }
    if (state->player2 != 0) {
       lws_write(state->player2, state->pos, sizeof(double)*3, LWS_WRITE_BINARY);
    }
}

int main() {
    // create context
    State* state = initState();
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = 9000;
    info.protocols = protocols;
    info.user = state;
    struct lws_context *context = lws_create_context(&info);
    if(!context) {
        lwsl_err("Failed to create WebSocket context\n");
        return -1;
    }
    
    signal(SIGINT, signal_handler);

    struct timeval t1, t2;
    double elapsedTime;




    //start timer
    gettimeofday(&t1, NULL);
    while(!interrupted) {
        
        time_t old_sec = t1.tv_sec;
        time_t old_usec = t1.tv_usec;
        gettimeofday(&t1, NULL);
        //compute
        elapsedTime = (t1.tv_sec - old_sec) * 1000.0;
        elapsedTime += (t1.tv_usec - old_usec ) / 1000.0;

        double dt = elapsedTime /1000;
        lws_service(context, 1000);

        moveSquare(state, dt);
        sendState(state);
    }
    lws_context_destroy(context);
    return 0;
}
