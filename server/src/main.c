#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "state.h"
#include "websocket.h"

static int interrupted = 0;

static void signal_handler() {
    interrupted = 1;
}

int main() {
    signal(SIGINT, signal_handler);
    struct lws_context *context = ws_create_context();
    while(!interrupted) {
        lws_service(context, 1000);
    }
    lws_context_destroy(context);
    return 0;
}
