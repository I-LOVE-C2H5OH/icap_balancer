#include "BufferManager.hpp"
BufferManager::BufferManager(): m_currentMessge(0)
{}

BufferManager::~BufferManager()
{
    for(auto buf : m_buffers)
    {
        buf->retrieveAll();
    }
}

void BufferManager::addBuffer(trantor::MsgBuffer* msgBuffer)
{ 
    std::lock_guard<std::mutex> guard(m_mutex);
    m_buffers.push_back(msgBuffer);
}

trantor::MsgBuffer* BufferManager::getLastBuffer()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_buffers[0];
} 

void BufferManager::sendet()
{
    m_currentMessge++;
}