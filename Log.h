#pragma once

#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <map>
#include <memory>
#include <vector>
#include <algorithm>

namespace loglib
{
    class Tag
    {
    public:
        virtual ~Tag() = default;
        std::string key() const
        {
            return mKey;
        }
        std::string text() const
        {
            return mText;
        }

        virtual std::unique_ptr<Tag> clone() const = 0;

    protected:
        Tag(std::string const &key, std::string const &value)
            : mKey(key), mText(key + "=\"" + value + "\"")
        {
        }

    private:
        std::string mKey;
        std::string const mText;
    };

    class LogLevel : public Tag
    {
    public:
        LogLevel(std::string const &text)
            : Tag("log_level", text)
        {
        }

        std::unique_ptr<Tag> clone() const override
        {
            return std::unique_ptr<Tag>(
                new LogLevel(*this));
        }
    };
    inline std::string to_string(Tag const &tag)
    {
        return tag.text();
    }

    inline std::map<std::string, std::unique_ptr<Tag>> &getDefaultTags()
    {
        static std::map<std::string, std::unique_ptr<Tag>> tags;
        return tags;
    }
    inline void addDefaultTag(Tag const &tag)
    {
        auto &tags = getDefaultTags();
        tags[tag.key()] = tag.clone();
    }

    inline std::fstream log(std::vector<Tag const *> tags = {})
    {
        const auto now = std::chrono::system_clock::now();
        std::time_t const tmNow = std::chrono::system_clock::to_time_t(now);
        auto const ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::fstream logFile("application.log", std::ios::app);
        logFile << std::endl;
        logFile << std::put_time(std::gmtime(&tmNow), "%Y-%m-%dT%H:%M:%S.") << std::setw(3) << std::setfill('0') << std::to_string(ms.count()) << " ";
        for (auto const &defaultTag : getDefaultTags())
        {
            if (std::find_if(tags.begin(), tags.end(),
                             [&defaultTag](auto const &tag)
                             {
                                 return defaultTag.first == tag->key();
                             }) == tags.end())
            {
                logFile << " " << defaultTag.second->text();
            }
        }
        for (auto const &tag : tags)
        {
            logFile << " " << tag->text();
        }
        logFile << " ";
        return logFile;
    }

    inline auto log(Tag const &tag1)
    {
        return log({&tag1});
    }
    inline auto log(Tag const &tag1,
                    Tag const &tag2)
    {
        return log({&tag1, &tag2});
    }
    inline auto log(Tag const &tag1,
                    Tag const &tag2,
                    Tag const &tag3)
    {
        return log({&tag1, &tag2, &tag3});
    }

}
