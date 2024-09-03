#pragma once
#include <trantor/net/TcpClient.h>
#include <trantor/net/EventLoopThread.h>
#include <memory>
#include "../BufferManager/BufferManager.hpp"
#include <mutex>

class MyTcpClient{
public:
    MyTcpClient(const std::string& remoteHost, int port, const trantor::TcpConnectionPtr& serverConnectionToClient, trantor::EventLoop *loop);
    ~MyTcpClient();
    void ServerRecvCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer);
    void ClientMessageCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer);
    void ClientConnectionCallback(const trantor::TcpConnectionPtr &conn);
    bool SendToClient(const std::string& buffer);
    bool SendToServer(const std::string& buffer);
private:
    std::mutex m_mutex;
    BufferManager m_clientBuffers;
    BufferManager m_serverBuffers;
    std::string m_remoteHost;
    trantor::InetAddress m_inetAddr;
    std::shared_ptr<trantor::TcpClient> m_client;
    trantor::TcpConnectionPtr m_serverConnectionToClient;
    trantor::TcpConnectionPtr m_serverConnectionToServer;
    trantor::EventLoop* m_eventLoop;
};