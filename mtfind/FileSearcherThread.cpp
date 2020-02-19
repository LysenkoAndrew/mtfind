#include "FileSearcherThread.h"

CFileSearcherThread::CFileSearcherThread() : m_bClose(true)
{
    m_bBusy.store(false);
}

CFileSearcherThread::~CFileSearcherThread()
{
    Close();
}

bool CFileSearcherThread::Start()
{
    std::lock_guard<std::mutex> lk(m_cvMutex);
    if (!m_bClose)
    {
        return false;
    }
    m_bClose = false;
    m_thread = std::thread(&CFileSearcherThread::ThreadRoutine, this);
    return true;
}

void CFileSearcherThread::ThreadRoutine()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(m_cvMutex);
        m_cvData.wait(lk, [this] {return !m_dataStr.empty() || m_bClose; });

        if (m_bClose)
        {
            return;
        }

        ProcessData();
        m_bBusy.store(false);
    }
}

void CFileSearcherThread::Close()
{
    if (m_thread.joinable())
    {
        while (m_bBusy.load());
        std::unique_lock<std::mutex> lk(m_cvMutex);
        m_bClose = true;
        m_cvData.notify_one();
        lk.unlock();
        m_thread.join();
        m_bBusy.store(false);
    }
}

bool CFileSearcherThread::TrySetData(const std::string& dataStr, unsigned dataNumber)
{
    bool bNew = false;
    if (m_bBusy.compare_exchange_strong(bNew, true, std::memory_order::memory_order_acquire))
    {
        std::lock_guard<std::mutex> lk(m_cvMutex);
        m_dataStr = dataStr;
        m_dataNumber = dataNumber;
        m_cvData.notify_one();
        return true;
    }
    return false;
}

void CFileSearcherThread::ProcessData()
{
    if (!m_dataStr.empty() && m_pCbProcessData)
    {
        m_pCbProcessData(m_dataNumber, m_dataStr);
    }
    m_dataStr.clear();
}

void CFileSearcherThread::SetCallBack(ThreadCallBack cb)
{
    m_pCbProcessData = cb;
}