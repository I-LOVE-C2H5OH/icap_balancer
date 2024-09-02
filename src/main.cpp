#include <trantor/net/TcpServer.h>
#include <trantor/utils/Logger.h>
#include <trantor/net/EventLoopThread.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include "TcpClient/TcpClient.hpp"

#define USE_IPV6 0
#define ICAP_PORT 1344
#define RemoteHost "10.4.46.15"
std::queue<trantor::TcpConnectionPtr> queue;

MyTcpClient* TcpClient;
using MyTcpClientPtr = std::shared_ptr<MyTcpClient>;
void processICAPRequest(const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
}

    trantor::EventLoop loopThread;
std::unordered_map<trantor::TcpConnectionPtr, MyTcpClientPtr> test;
int main() {
    LOG_DEBUG << "ICAP server start";
    trantor::InetAddress addr(1344);
    trantor::TcpServer server(&loopThread, addr, "ICAPServer");

    server.setBeforeListenSockOptCallback([](int fd) {
        std::cout << "setBeforeListenSockOptCallback:" << fd << std::endl;
    });

    server.setAfterAcceptSockOptCallback([](int fd) {
        std::cout << "afterAcceptSockOptCallback:" << fd << std::endl;
    });

    server.setRecvMessageCallback(
        [](const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer) {
            LOG_DEBUG << "Receive message callback";
            test[connectionPtr]->ServerRecvCallback(connectionPtr, buffer);
        }
    );

    server.setConnectionCallback([](const trantor::TcpConnectionPtr &connPtr) {
        if (connPtr->connected()) {
            test[connPtr] = std::make_shared<MyTcpClient>(RemoteHost, ICAP_PORT, connPtr, &loopThread);
            LOG_DEBUG << "New ICAP connection";
        } else if (connPtr->disconnected()) {
            test[connPtr].reset();
            LOG_DEBUG << "ICAP connection disconnected";
        }
    });

    server.setIoLoopNum(3);
    server.start();
    loopThread.loop();
}