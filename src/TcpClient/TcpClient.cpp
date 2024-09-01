#include "TcpClient.hpp"
MyTcpClient::MyTcpClient(std::string remoteHost, int port, const trantor::TcpConnectionPtr& serverConnectionToClient, trantor::EventLoop *loop) :
    m_serverConnectionToClient(serverConnectionToClient),
    m_remoteHost(remoteHost),
    m_eventLoop(loop), 
    m_inetAddr("10.4.46.15", 1344) 
{
    // Устанавливаем колбек для получения сообщений от сервера
    m_serverConnectionToClient->setRecvMsgCallback([this](const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
        this->ServerRecvCallback(connectionPtr, buffer);
    });

    m_client = std::make_unique<trantor::TcpClient>(m_eventLoop, m_inetAddr, m_remoteHost);

    // Устанавливаем колбеки для TcpClient
    m_client->setMessageCallback([this](const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
        this->ClientMessageCallback(connectionPtr, buffer);
    });
    
    m_client->setConnectionCallback([this](const trantor::TcpConnectionPtr &conn) {
        ClientConnectionCallback(conn);
    });

    m_client->connect();
}

MyTcpClient::~MyTcpClient()
{
    m_client->disconnect();
    for(auto buf : m_clientBuffers)
    {
        buf->retrieveAll();
    }for(auto buf : m_serverBuffers)
    {
        buf->retrieveAll();
    }
}

void MyTcpClient::ServerRecvCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer)
{
    m_serverBuffers.push_back(buffer);
    m_serverConnectionToServer->send(*buffer);
}

void MyTcpClient::ClientMessageCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer)
{
    m_clientBuffers.push_back(buffer);
    m_serverConnectionToServer->send(*buffer);
}

void MyTcpClient::ClientConnectionCallback(const trantor::TcpConnectionPtr &connectionPtr)
{
    m_serverConnectionToServer = connectionPtr;
    if(m_serverConnectionToServer->connected())
    {
    }
}