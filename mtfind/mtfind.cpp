// mtfind.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "FileSearcher.h"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Parameters should be: mtfind.exe <file path> <mask>\n";
        return 1;
    }

    const std::string filePath = argv[1];
    const std::string mask = argv[2];

    try
    {
        CFileSearcher fileSearcher(filePath, mask);
        fileSearcher.Start();
        fileSearcher.GetResults();
    }
    catch (std::invalid_argument& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

