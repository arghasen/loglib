#include "../Log.h"
#include "Util.h"
#include "LogTags.h"

#include <TDDLIB/Test.h>

TEST("Message can be tagged in Log")
{
    std::string message = "simple tag ";
    message += Util::randomString();
    loglib::log(error) << message;
    std::string logLevelTag = " log_level=\"error\" ";
    std::string defaultLogLevelTag = " log_level=\"info\" ";
    bool result = Util::isTextInFile(message, "application.log",{logLevelTag},{defaultLogLevelTag});
    CONFIRM_TRUE(result);
}

TEST("log needs no namespace when used with LogLevel")
{
    log(error) << "no namespace";
}

TEST("Default tags set in main appear in log")
{
    std::string message = "default tag ";
    message += Util::randomString();
    loglib::log() << message;
    std::string logLevelTag = " log_level=\"info\" ";
    std::string colorTag = " color=\"green\" ";
    bool result = Util::isTextInFile(message, "application.log",
                                     {logLevelTag, colorTag});
    CONFIRM_TRUE(result);
}