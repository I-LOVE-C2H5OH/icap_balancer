#include "BufferManager.hpp"
BufferManager::BufferManager(): m_currentMessge(0)
{}

BufferManager::~BufferManager()
{
}

void BufferManager::addBuffer(trantor::MsgBuffer* msgBuffer)
{ 
    std::lock_guard<std::mutex> guard(m_mutex);
    m_msgs.push_back(msgBuffer->read(msgBuffer->readableBytes()));
}

std::string BufferManager::getLastBuffer()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    //return m_buffers[0];
    if(m_msgs.empty())
    {
        return "";
    }
    return m_msgs[m_msgs.size() - 1];
} 

void BufferManager::sendet()
{
}