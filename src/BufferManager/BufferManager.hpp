#pragma once
#include <vector>
#include <trantor/utils/MsgBuffer.h>
#include <mutex>
class BufferManager
{
public:
BufferManager();
~BufferManager();
void addBuffer(trantor::MsgBuffer* msgBuffer);
void sendet();
std::mutex m_mutex;
std::string getLastBuffer();
private:
std::size_t m_currentMessge;
std::vector<std::string> m_msgs;
};