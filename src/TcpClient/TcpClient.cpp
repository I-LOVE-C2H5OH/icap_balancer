#include "TcpClient.hpp"
TcpClient::TcpClient(std::string remoteHost,trantor::TcpConnectionPtr serverConnectionToClient) : 
m_serverConnectionToClient(serverConnectionToClient)
, m_remoteHost(remoteHost)
{

}