#include <trantor/net/TcpServer.h>
#include <trantor/utils/Logger.h>
#include <trantor/net/EventLoopThread.h>
#include <string>
#include <iostream>

using namespace trantor;
#define USE_IPV6 0

std::queue<trantor::TcpConnectionPtr> queue;

void processICAPRequest(const TcpConnectionPtr& connectionPtr, MsgBuffer* buffer) {
}

int main() {
    LOG_DEBUG << "ICAP server start";
    EventLoopThread loopThread;
    loopThread.run();
    InetAddress addr(1344);
    TcpServer server(loopThread.getLoop(), addr, "ICAPServer");

    server.setBeforeListenSockOptCallback([](int fd) {
        std::cout << "setBeforeListenSockOptCallback:" << fd << std::endl;
    });

    server.setAfterAcceptSockOptCallback([](int fd) {
        std::cout << "afterAcceptSockOptCallback:" << fd << std::endl;
    });

    server.setRecvMessageCallback(
        [](const TcpConnectionPtr &connectionPtr, MsgBuffer *buffer) {
            LOG_DEBUG << "Receive message callback";
            processICAPRequest(connectionPtr, buffer);
        }
    );

    server.setConnectionCallback([](const TcpConnectionPtr &connPtr) {
        if (connPtr->connected()) {
            LOG_DEBUG << "New ICAP connection";
        } else if (connPtr->disconnected()) {
            LOG_DEBUG << "ICAP connection disconnected";
        }
    });

    server.setIoLoopNum(3);
    server.start();
    loopThread.wait();
}