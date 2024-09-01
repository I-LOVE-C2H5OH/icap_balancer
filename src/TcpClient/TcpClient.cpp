#include "TcpClient.hpp"
TcpClient::TcpClient(std::string remoteHost, int port, const trantor::TcpConnectionPtr& serverConnectionToClient) : 
m_serverConnectionToClient(serverConnectionToClient)
, m_remoteHost(remoteHost)
{
    m_serverConnectionToClient->setRecvMsgCallback([this](const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
            this->ServerRecvCallback(connectionPtr, buffer);
        });
    m_client = std::make_unique<trantor::TcpClient>(m_eventLoopThread->getLoop() ,m_inetAddr, m_remoteHost);
    m_client->setMessageCallback([this](const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
            this->ClientMessageCallback(connectionPtr, buffer);
        });
}

TcpClient::~TcpClient()
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

void TcpClient::ServerRecvCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer)
{
    m_serverBuffers.push_back(buffer);
}

void TcpClient::ClientMessageCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer)
{
    m_clientBuffers.push_back(buffer);
}