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
    bool result = Util::isTextInFile(message, "application.log", {logLevelTag}, {defaultLogLevelTag});
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

TEST("Multiple tags can be used in log")
{
    std::string message = "multi tags ";
    message += Util::randomString();
    loglib::log(debug, red, large) << message;
    std::string logLevelTag = " log_level=\"debug\" ";
    std::string colorTag = " color=\"red\" ";
    std::string sizeTag = " size=\"large\" ";
    bool result = Util::isTextInFile(message, "application.log",
                                     {logLevelTag, colorTag, sizeTag});
    CONFIRM_TRUE(result);
}

TEST("Tags can be streamed to log")
{
    std::string messageBase = " 1 type ";
    std::string message = messageBase + Util::randomString();
    loglib::log(info) << Count(1) << message;
    std::string countTag = " count=1 ";
    bool result = Util::isTextInFile(message, "application.log", {countTag});
    CONFIRM_TRUE(result);
    messageBase = " 2 type ";
    message = messageBase + Util::randomString();
    loglib::log(info) << Identity(123456789012345) << message;
    std::string idTag = " id=123456789012345 ";
    result = Util::isTextInFile(message, "application.log", {idTag});
    CONFIRM_TRUE(result);
    messageBase = " 3 type ";
    message = messageBase + Util::randomString();
    loglib::log(info) << Scale(1.5) << message;
    std::string scaleTag = " scale=1.500000 ";
    result = Util::isTextInFile(message, "application.log", {scaleTag});
    CONFIRM_TRUE(result);
    messageBase = " 4 type ";
    message = messageBase + Util::randomString();
    loglib::log(info) << cacheMiss << message;
    std::string cacheTag = " cache_hit=false ";
    result = Util::isTextInFile(message, "application.log", {cacheTag});
    CONFIRM_TRUE(result);
}
class TempFilterClause
{
public:
    void setup()
    {
        mId = loglib::createFilterClause();
    }
    void teardown()
    {
        loglib::clearFilterClause(mId);
    }
    int id() const
    {
        return mId;
    }

private:
    int mId;
};

TEST("Tags can be used to filter messages")
{
    int id = loglib::createFilterClause();
    loglib::addFilterLiteral(id, error);
    std::string message = "filter ";
    message += Util::randomString();
    loglib::log(info) << message;
    bool result = Util::isTextInFile(message, "application.log");
    CONFIRM_FALSE(result);
    loglib::clearFilterClause(id);
    loglib::log(info) << message;
    result = Util::isTextInFile(message, "application.log");
    CONFIRM_TRUE(result);
}

TEST("Overridden default tag not used to filter messages")
{
    TDDLib::SetupAndTeardown<TempFilterClause> filter;
    loglib::addFilterLiteral(filter.id(), info);
    std::string message = "override default ";
    message += Util::randomString();
    loglib::log(debug) << message;
    bool result = Util::isTextInFile(message, "application.log");
    CONFIRM_FALSE(result);
}

TEST("Inverted tag can be used to filter messages")
{
    TDDLib::SetupAndTeardown<TempFilterClause> filter;
    loglib::addFilterLiteral(filter.id(), green, false);
    std::string message = "inverted ";
    message += Util::randomString();
    loglib::log(info) << message;
    bool result = Util::isTextInFile(message, "application.log");
    CONFIRM_FALSE(result);
}

TEST("Tag values can be used to filter messages")
{
    TDDLib::SetupAndTeardown<TempFilterClause> filter;
    loglib::addFilterLiteral(filter.id(),
                             Count(100, loglib::TagOperation::GreaterThan));
    std::string message = "values ";
    message += Util::randomString();
    loglib::log(Count(1)) << message;
    bool result = Util::isTextInFile(message, "application.log");
    CONFIRM_FALSE(result);
    loglib::log() << Count(101) << message;
    result = Util::isTextInFile(message, "application.log");
    CONFIRM_FALSE(result);
    loglib::log(Count(101)) << message;
    result = Util::isTextInFile(message, "application.log");
    CONFIRM_TRUE(result);
}