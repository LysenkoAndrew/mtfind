#include "FileSearcher.h"
#include <iostream>
#include <regex>

CFileSearcher* CFileSearcher::m_pThis = nullptr;

CFileSearcher::CFileSearcher(const std::string& inputFilePath, const std::string& mask) :
    m_mask(mask), m_nMaxThread(std::thread::hardware_concurrency())
{
    m_inFile.open(inputFilePath, std::ios_base::in);
    CheckSearchMask();
    m_pThis = this;
}

CFileSearcher::~CFileSearcher()
{
    if (m_inFile.is_open())
    {
        m_inFile.close();
    }
}

void CFileSearcher::Start()
{
    if (!m_inFile.is_open())
    {
        throw std::invalid_argument("Error: Cannot open input file.");
    }

    std::string str;
    unsigned strNumber = 1;
    while (std::getline(m_inFile, str))
    {
        if (!str.empty())
        {
            if (m_threads.size() < m_nMaxThread)
            {
                // создаем новый поток
                m_threads.emplace_back();
                auto& thread = m_threads.back();
                thread.SetCallBack(Collect);
                if (!thread.Start())
                {
                    throw std::exception("Error: Cannot open thread!");
                }
                else
                {
                    thread.TrySetData(str, strNumber);
                }
            }
            else
            {
                bool bDone = false;
                do
                {
                    for (auto& th : m_threads)
                    {
                        if (bDone = th.TrySetData(str, strNumber))
                        {
                            break;
                        }
                    }
                } while (!bDone);
            }
            strNumber++;
        }
    }
}

void CFileSearcher::CheckSearchMask()
{
    const size_t maskLength = m_mask.length();
    if (m_mask.empty())
    {
        throw std::length_error("Error: Mask is empty!");
    }
    else if (m_mask.length() > 100)
    {
        throw std::length_error("Error: Mask length is too big!");
    }
    else if (std::string::npos != m_mask.find('\n'))
    {
        throw std::logic_error("Error: The mask must not contain a newline character!");
    }
    else if (std::string::npos == m_mask.find('?'))
    {
        throw std::logic_error("Error: Mask invalid!");
    }
    std::replace(m_mask.begin(), m_mask.end(), '?', '.');
}

void CFileSearcher::Collect(unsigned strNumber, std::string str)
{
    if (m_pThis)
    {
        std::regex reg(m_pThis->m_mask);
        std::smatch match;
        if (std::regex_search(str, match, reg))
        {
            std::lock_guard<std::mutex> lock(m_pThis->m_listMutex);
            m_pThis->m_threadInput.emplace(strNumber, std::make_pair(static_cast<unsigned>(match.position(0)) + 1, match.str(0)));
        }
    }
}

void CFileSearcher::GetResults()
{
    std::for_each(m_threads.begin(), m_threads.end(), [](CFileSearcherThread& thread) { thread.Close(); });
    std::lock_guard<std::mutex> lock(m_listMutex);
    std::cout << m_threadInput.size() << std::endl;
    for (const auto& elem : m_threadInput)
    {
        std::cout << elem.first << " " << elem.second.first << " " << elem.second.second << std::endl;
    }
}