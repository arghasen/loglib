#include "LogTags.h"

#include <TDDLIB/Test.h>

#include <iostream>

int main()
{
    loglib::FileOutput appFile("logs");
    // appFile.namePattern() = "application-{}.log";
    // appFile.maxSize() = 10'000'000;
    // appFile.rolloverCount() = 5;
    loglib::addLogOutput(appFile);
    loglib::StreamOutput consoleStream(std::cout);
    loglib::addLogOutput(consoleStream);
    loglib::addDefaultTag(info);
    loglib::addDefaultTag(green);
    return TDDLib::runTests(std::cout);
}