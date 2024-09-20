#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>

static int interrupted = 0;

static void signal_handler() {
    interrupted = 1;
}

static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason,
        void *user, void *in, size_t len) {
    switch(reason) {
        case LWS_CALLBACK_RECEIVE:
            printf("echo");
            break;
        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "echo-protocol",
        callback_echo,
        0, // per session data size
        1024, // max frame size
    },
    { NULL, NULL, 0, 0 } // null terminate
};

int main() {
    // create context
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = 9000;
    info.protocols = protocols;
    struct lws_context *context = lws_create_context(&info);
    if(!context) {
        lwsl_err("Failed to create WebSocket context\n");
        return -1;
    }
    signal(SIGINT, signal_handler);
    while(!interrupted) {
        lws_service(context, 1000);
    }
    lws_context_destroy(context);
    return 0;
}
