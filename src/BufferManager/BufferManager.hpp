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
trantor::MsgBuffer* getLastBuffer();
std::vector<trantor::MsgBuffer*> getAllBuffers;
private:
std::size_t m_currentMessge;
std::vector<trantor::MsgBuffer*> m_buffers;
};