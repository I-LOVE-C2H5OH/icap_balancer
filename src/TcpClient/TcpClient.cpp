#include "TcpClient.hpp"
MyTcpClient::MyTcpClient(std::string remoteHost, int port, const trantor::TcpConnectionPtr& serverConnectionToClient, trantor::EventLoop *loop) :
    m_serverConnectionToClient(serverConnectionToClient),
    m_remoteHost(remoteHost),
    m_inetAddr("10.4.46.15", 1344),
    m_eventLoop(loop)
{
    // Устанавливаем колбек для получения сообщений от сервера
    m_serverConnectionToClient->setRecvMsgCallback([this](const trantor::TcpConnectionPtr& connectionPtr, trantor::MsgBuffer* buffer) {
        this->ServerRecvCallback(connectionPtr, buffer);
    });

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
    m_clientBuffers.addBuffer(buffer);
    if(auto lastBuff = m_clientBuffers.getLastBuffer())
    {
        if(SendToServer(lastBuff))
        {
            m_clientBuffers.sendet();
        }
    }
}

void MyTcpClient::ClientMessageCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer)
{
    m_serverBuffers.addBuffer(buffer);
    if(SendToClient(buffer))
    {
        m_serverBuffers.sendet();
    }
}

void MyTcpClient::ClientConnectionCallback(const trantor::TcpConnectionPtr &connectionPtr)
{
    m_serverConnectionToServer = connectionPtr;
    if(m_serverConnectionToServer->connected())
    {
        auto lastBuff = m_clientBuffers.getLastBuffer();
        if(lastBuff)
        {
            SendToServer(lastBuff);
        }
    }
}

bool MyTcpClient::SendToClient(trantor::MsgBuffer *buffer)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    std::string sended = buffer->beginWrite();
    if(m_serverConnectionToClient){
        m_serverConnectionToClient->send(*buffer);
        buffer->retrieveAll();
        return true;
    }
    return false;
}

bool MyTcpClient::SendToServer(trantor::MsgBuffer *buffer)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if(m_serverConnectionToServer){
        m_serverConnectionToServer->send(*buffer);
        isSendet = true;
        buffer->retrieveAll();
        return true;
    }
    return false;
}