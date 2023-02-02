#include "../Log.h"
#include "Util.h"

#include <TDDLIB/Test.h>

TEST("Simple message can be logged")
{
    std::string message = "simple ";
    message += Util::randomString();
    loglib::log() << message << " with more text";
    bool result = Util::isTextInFile(message, "application.log");
    CONFIRM_TRUE(result);
}

TEST("Complicated message can be logged")
{
    std::string message = "complicated ";
    message += Util::randomString();
    loglib::log() << message
                  << " double=" << 3.14
                  << " quoted=" << std::quoted("in quotes");
    bool result = Util::isTextInFile(message, "application.log");
    CONFIRM_TRUE(result);
}