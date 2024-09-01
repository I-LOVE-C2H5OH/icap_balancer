#include <trantor/net/TcpClient.h>
#include <memory>
class TcpClient{
public:
    TcpClient(std::string remoteHost, trantor::TcpConnectionPtr serverConnectionToClient);
private:
    std::string m_remoteHost;
    std::unique_ptr<trantor::TcpClient> m_client;
    trantor::TcpConnectionPtr m_serverConnectionToClient;
};