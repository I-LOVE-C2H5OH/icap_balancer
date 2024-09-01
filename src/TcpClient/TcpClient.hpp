#include <trantor/net/TcpClient.h>
#include <trantor/net/EventLoopThread.h>
#include <memory>

class MyTcpClient{
public:
    MyTcpClient(std::string remoteHost, int port, const trantor::TcpConnectionPtr& serverConnectionToClient, trantor::EventLoop *loop);
    ~MyTcpClient();
    void ServerRecvCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer);
    void ClientMessageCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer);
    void ClientConnectionCallback(const trantor::TcpConnectionPtr &conn);
private:
    std::vector<trantor::MsgBuffer*> m_clientBuffers;
    std::vector<trantor::MsgBuffer*> m_serverBuffers;
    std::string m_remoteHost;
    trantor::InetAddress m_inetAddr;
    std::shared_ptr<trantor::TcpClient> m_client;
    trantor::TcpConnectionPtr m_serverConnectionToClient;
    trantor::TcpConnectionPtr m_serverConnectionToServer;
    trantor::EventLoop* m_eventLoop;
};