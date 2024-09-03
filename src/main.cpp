#include <queue>
#include <trantor/net/TcpServer.h>
#include <trantor/utils/Logger.h>
#include <trantor/net/EventLoopThread.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include "TcpClient/TcpClient.hpp"
#include "Setting/Setting.hpp"

std::unordered_map<trantor::TcpConnectionPtr, trantor::MsgBuffer*> messageMap;
std::deque<trantor::TcpConnectionPtr> pendingQueue;
MyTcpClient* TcpClient;
using MyTcpClientPtr = std::shared_ptr<MyTcpClient>;

void processICAPRequest(const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
}

trantor::EventLoop loopThread;
std::unordered_map<trantor::TcpConnectionPtr, MyTcpClientPtr> activeConnections;

void handlePendingConnections(const Setting& setting) {
    while (!pendingQueue.empty() && activeConnections.size() < setting.maxActiveDistanceConnections()) {
        auto connPtr = pendingQueue.front();
        pendingQueue.pop_front();
        activeConnections[connPtr] = std::make_shared<MyTcpClient>(setting.distanceHost(), setting.distancePort(), connPtr, &loopThread);
        LOG_DEBUG << "Pending ICAP connection activated";
        if(messageMap[connPtr])
        { 
            activeConnections[connPtr]->ServerRecvCallback(connPtr, messageMap[connPtr]);
            messageMap.erase(connPtr);
        }
    }
}

int main(int argc, char **argv) {
    if(argc != 2)
    {
        LOG_ERROR << "USE ./programm configureFile.yml";
        return 1;
    }
    Setting setting(argv[1]);
    LOG_DEBUG << "ICAP server start";
    trantor::InetAddress addr(setting.sourcePort());
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
    server.setConnectionCallback([&setting](const trantor::TcpConnectionPtr &connPtr) {
        if (connPtr->connected()) {
            if (activeConnections.size() < setting.maxActiveDistanceConnections()) {
                activeConnections[connPtr] = std::make_shared<MyTcpClient>(setting.distanceHost(), setting.distancePort(), connPtr, &loopThread);
                LOG_DEBUG << "New ICAP connection";
            } else {
                pendingQueue.push_back(connPtr);
                LOG_DEBUG << "Pending connection added to queue";
            }
        } else if (connPtr->disconnected()) {
            activeConnections.erase(connPtr);
            pendingQueue.erase(std::remove(pendingQueue.begin(), pendingQueue.end(), connPtr), pendingQueue.end());
            LOG_DEBUG << "ICAP connection disconnected";
            handlePendingConnections(setting);
        }
    });

    server.setIoLoopNum(5);
    server.start();
    loopThread.loop();
}