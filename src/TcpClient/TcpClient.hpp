#include <trantor/net/TcpClient.h>
#include <trantor/net/EventLoopThread.h>
#include <memory>

class TcpClient{
public:
    TcpClient(std::string remoteHost, int port, const trantor::TcpConnectionPtr& serverConnectionToClient);
    ~TcpClient();
    void ServerRecvCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer);
    void ClientMessageCallback(const trantor::TcpConnectionPtr &connectionPtr, trantor::MsgBuffer *buffer);
private:
    std::vector<trantor::MsgBuffer*> m_clientBuffers;
    std::vector<trantor::MsgBuffer*> m_serverBuffers;
    std::string m_remoteHost;
    trantor::InetAddress m_inetAddr;
    std::unique_ptr<trantor::TcpClient> m_client;
    trantor::TcpConnectionPtr m_serverConnectionToClient;
    std::shared_ptr<trantor::EventLoopThread> m_eventLoopThread;
};