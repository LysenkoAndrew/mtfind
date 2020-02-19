#pragma once
#include <string>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>

using ThreadCallBack = void(*)(unsigned, std::string);
class CFileSearcherThread
{
public:
    CFileSearcherThread();
    virtual ~CFileSearcherThread();
    CFileSearcherThread(const CFileSearcherThread&) = delete;
    CFileSearcherThread& operator=(CFileSearcherThread&) = delete;

    bool Start();
    void Close();
    bool TrySetData(const std::string& dataStr, unsigned dataNumber);
    void ThreadRoutine();
    void SetCallBack(ThreadCallBack cb);

protected:
    virtual void ProcessData();

private:
    std::thread m_thread;
    std::condition_variable m_cvData;
    std::mutex	m_cvMutex;
    std::atomic_bool m_bBusy;
    bool m_bClose;
    std::string m_dataStr;
    unsigned m_dataNumber;
    ThreadCallBack m_pCbProcessData;
};
