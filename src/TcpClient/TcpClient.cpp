#include "TcpClient.hpp"
MyTcpClient::MyTcpClient(std::string remoteHost, int port, const trantor::TcpConnectionPtr& serverConnectionToClient, trantor::EventLoop *loop) :
    m_serverConnectionToClient(serverConnectionToClient),
    m_remoteHost(remoteHost),
    m_inetAddr("10.4.46.15", 1344),
    m_eventLoop(loop)
{
    m_client = std::make_shared<trantor::TcpClient>(m_eventLoop, m_inetAddr, m_remoteHost);

    // Устанавливаем колбеки для TcpClient
    m_client->setMessageCallback([this](const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
        ClientMessageCallback(connectionPtr, buffer);
    });
    
    m_client->setConnectionCallback([this](const trantor::TcpConnectionPtr &conn) {
        ClientConnectionCallback(conn);
    });
    m_client->connect();
}

MyTcpClient::~MyTcpClient()
{
    m_client->disconnect();
}

void MyTcpClient::ServerRecvCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_clientBuffers.addBuffer(buffer);
    {
        SendToServer(m_clientBuffers.getLastBuffer());
    }
}

void MyTcpClient::ClientMessageCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_serverBuffers.addBuffer(buffer);
    SendToClient(m_serverBuffers.getLastBuffer());
}

void MyTcpClient::ClientConnectionCallback(const trantor::TcpConnectionPtr &connectionPtr)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_serverConnectionToServer = connectionPtr;
    if(m_serverConnectionToServer->connected())
    {
        SendToServer(m_clientBuffers.getLastBuffer());
    }
}

bool MyTcpClient::SendToClient(const std::string& buffer)
{
    if(m_serverConnectionToClient && !buffer.empty()){
        m_serverConnectionToClient->send(buffer);
        return true;
    }
    return false;
}

bool MyTcpClient::SendToServer(const std::string& buffer)
{
    if(m_serverConnectionToServer && !buffer.empty()){
        m_serverConnectionToServer->send(buffer);
        isSendet = true;
        return true;
    }
    return false;
}