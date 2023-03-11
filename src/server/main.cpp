#include "chatserver.h"
#include "service.h"
#include <signal.h>
#include <iostream>

void fun(int sig)
{
    Service::getinstance()->reset();
    exit(0);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "./server ip port\n";
        exit(-1);
    }

    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    struct sigaction sa;
    sa.sa_handler = fun;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, nullptr);

    EventLoop loop;
    InetAddress serveraddr(ip, port);
    ChatServer server(&loop, serveraddr, "chatserver");

    server.start();
    loop.loop();

    return 0;
}