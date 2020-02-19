#pragma once
#include <string>
#include <list>
#include <map>
#include <fstream>
#include <mutex>
#include "FileSearcherThread.h"

class CFileSearcher
{
public:
    CFileSearcher(const std::string& inputFilePath, const std::string& mask);
    virtual ~CFileSearcher();
    CFileSearcher(const CFileSearcher&) = delete;
    CFileSearcher& operator=(CFileSearcher&) = delete;

    void Start();
    virtual void GetResults();
protected:
    virtual void CheckSearchMask();
    static void Collect(unsigned, std::string);
    static CFileSearcher* m_pThis;
private:
    const unsigned m_nMaxThread;
    std::list<CFileSearcherThread> m_threads;

    std::mutex m_listMutex;
    std::map<unsigned, std::pair<unsigned, std::string>> m_threadInput;

    std::ifstream m_inFile;
    std::string m_mask;
};