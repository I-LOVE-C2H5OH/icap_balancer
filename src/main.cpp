#include <queue>
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
constexpr size_t MAX_ACT_CONN = 4;

std::unordered_map<trantor::TcpConnectionPtr, trantor::MsgBuffer*> messageMap;
std::deque<trantor::TcpConnectionPtr> pendingQueue;
MyTcpClient* TcpClient;
using MyTcpClientPtr = std::shared_ptr<MyTcpClient>;

void processICAPRequest(const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
}

trantor::EventLoop loopThread;
std::unordered_map<trantor::TcpConnectionPtr, MyTcpClientPtr> activeConnections;

void handlePendingConnections() {
    while (!pendingQueue.empty() && activeConnections.size() < MAX_ACT_CONN) {
        auto connPtr = pendingQueue.front();
        pendingQueue.pop_front();
        activeConnections[connPtr] = std::make_shared<MyTcpClient>(RemoteHost, ICAP_PORT, connPtr, &loopThread);
        LOG_DEBUG << "Pending ICAP connection activated";
        if(messageMap[connPtr])
        { 
            activeConnections[connPtr]->ServerRecvCallback(connPtr, messageMap[connPtr]);
            messageMap.erase(connPtr);
        }
    }
}

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
            if (activeConnections.find(connectionPtr) != activeConnections.end()) {
                activeConnections[connectionPtr]->ServerRecvCallback(connectionPtr, buffer);
            }
            else {
                messageMap[connectionPtr] = buffer;
            }
        }
    );

    server.setConnectionCallback([](const trantor::TcpConnectionPtr &connPtr) {
        if (connPtr->connected()) {
            if (activeConnections.size() < MAX_ACT_CONN) {
                activeConnections[connPtr] = std::make_shared<MyTcpClient>(RemoteHost, ICAP_PORT, connPtr, &loopThread);
                LOG_DEBUG << "New ICAP connection";
            } else {
                pendingQueue.push_back(connPtr);
                LOG_DEBUG << "Pending connection added to queue";
            }
        } else if (connPtr->disconnected()) {
            activeConnections.erase(connPtr);
            pendingQueue.erase(std::remove(pendingQueue.begin(), pendingQueue.end(), connPtr), pendingQueue.end());
            LOG_DEBUG << "ICAP connection disconnected";
            handlePendingConnections();
        }
    });

    server.setIoLoopNum(3);
    server.start();
    loopThread.loop();
}