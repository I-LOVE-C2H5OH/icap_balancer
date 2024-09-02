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
constexpr size_t MAX_ACT_CONN = 10;

std::queue<trantor::TcpConnectionPtr> pendingQueue;
std::unordered_map<trantor::TcpConnectionPtr, std::vector<trantor::MsgBuffer>> pendingMessages;
MyTcpClient* TcpClient;
using MyTcpClientPtr = std::shared_ptr<MyTcpClient>;

void processICAPRequest(const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
    // Проверяем активность соединения
    if (pendingMessages.find(connectionPtr) != pendingMessages.end()) {
        // Если соединение заморожено, добавляем сообщение в очередь
        pendingMessages[connectionPtr].push_back(*buffer);
    } else {
        // Обработка сообщений для активных соединений
        if (activeConnections.find(connectionPtr) != activeConnections.end()) {
            activeConnections[connectionPtr]->send(buffer);
        }
    }
}

trantor::EventLoop loopThread;
std::unordered_map<trantor::TcpConnectionPtr, MyTcpClientPtr> activeConnections;

void handlePendingConnections() {
    while (!pendingQueue.empty() && activeConnections.size() < MAX_ACT_CONN) {
        auto connPtr = pendingQueue.front();
        pendingQueue.pop();
        activeConnections[connPtr] = std::make_shared<MyTcpClient>(RemoteHost, ICAP_PORT, connPtr, &loopThread);
        LOG_DEBUG << "Pending ICAP connection activated";

        // Перемещение сообщений из очереди в активное соединение
        if (pendingMessages.find(connPtr) != pendingMessages.end()) {
            for (auto& msg : pendingMessages[connPtr]) {
                activeConnections[connPtr]->send(&msg);
            }
            pendingMessages.erase(connPtr);
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

    server.setRecvMessageCallback([](const trantor::TcpConnectionPtr& connPtr, trantor::MsgBuffer* buffer) {
        processICAPRequest(connPtr, buffer);
    });

    server.setConnectionCallback([](const trantor::TcpConnectionPtr& connPtr) {
        if (connPtr->connected()) {
            if (activeConnections.size() >= MAX_ACT_CONN) {
                pendingQueue.push(connPtr);
                pendingMessages[connPtr]; // Создаем пустую очередь сообщений для замороженного соединения
                LOG_DEBUG << "Received new ICAP connection but maximum active connections reached. Connection is in pending state.";
            } else {
                activeConnections[connPtr] = std::make_shared<MyTcpClient>(RemoteHost, ICAP_PORT, connPtr, &loopThread);
                LOG_DEBUG << "Received new ICAP connection";
            }
        } else {
            // Удаляем соединение из карты активных соединений
            activeConnections.erase(connPtr);
            pendingMessages.erase(connPtr);
            handlePendingConnections();
        }
    });

    server.start();
    loopThread.loop();
    return 0;
}