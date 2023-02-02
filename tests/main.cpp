#include "LogTags.h"

#include <TDDLIB/Test.h>

#include <iostream>

int main()
{
    loglib::addDefaultTag(info);
    loglib::addDefaultTag(green);
    return TDDLib::runTests(std::cout);
}