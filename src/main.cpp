#include <trantor/net/TcpServer.h>
#include <trantor/utils/Logger.h>
#include <trantor/net/EventLoopThread.h>
#include <string>
#include <iostream>

using namespace trantor;
#define USE_IPV6 0

void processICAPRequest(const TcpConnectionPtr& connectionPtr, MsgBuffer* buffer) {
    std::string receivedData(buffer->peek(), buffer->readableBytes());
    std::cout << "Received ICAP request: " << receivedData << std::endl;

    if (receivedData.find("OPTIONS") != std::string::npos) {
        std::string response = "ICAP/1.0 200 OK\r\n"
                               "Methods: RESPMOD REQMOD OPTIONS\r\n"
                               "Service: ExampleICAPServer\r\n"
                               "Transfer-Preview: *\r\n"
                               "Preview: 1024\r\n"
                               "Max-Connections: 100\r\n"
                               "Options-TTL: 3600\r\n"
                               "Date: Mon, 30 Nov 2023 01:01:01 GMT\r\n\r\n";
        connectionPtr->send(response.data(), response.size());
    } else if (receivedData.find("REQMOD") != std::string::npos) {
        // Process REQMOD request here
        std::string response = "ICAP/1.0 200 OK\r\n"
                               "Encapsulated: req-hdr=0, null-body=" + std::to_string(receivedData.size()) + "\r\n"
                               "\r\n";
        connectionPtr->send(response.data(), response.size());
    } else if (receivedData.find("RESPMOD") != std::string::npos) {
        // Process RESPMOD request here
        std::string response = "ICAP/1.0 200 OK\r\n"
                               "Encapsulated: res-hdr=0, res-body=" + std::to_string(receivedData.size()) + "\r\n"
                               "\r\n";
        connectionPtr->send(response.data(), response.size());
    } else {
        std::string response = "ICAP/1.0 400 Bad Request\r\n"
                               "Date: Mon, 30 Nov 2023 01:01:01 GMT\r\n"
                               "\r\n";
        connectionPtr->send(response.data(), response.size());
    }

    buffer->retrieveAll();
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