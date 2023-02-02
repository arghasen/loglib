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
        Tag(std::string const &key, int value)
            : mKey(key), mText(key + "=" + std::to_string(value))
        {
        }
        Tag(std::string const &key, long long value)
            : mKey(key), mText(key + "=" + std::to_string(value))
        {
        }
        Tag(std::string const &key, double value)
            : mKey(key), mText(key + "=" + std::to_string(value))
        {
        }
        Tag(std::string const &key, bool value)
            : mKey(key), mText(key + "=" + (value ? "true" : "false"))
        {
        }

    private:
        std::string mKey;
        std::string const mText;
    };

    template <typename T, typename ValueT>
    class TagType : public Tag
    {
    public:
        std::unique_ptr<Tag> clone() const override
        {
            return std::unique_ptr<Tag>(
                new T(*static_cast<T const *>(this)));
        }
        ValueT value() const
        {
            return mValue;
        }

    protected:
        TagType(ValueT const &value)
            : Tag(T::key, value), mValue(value)
        {
        }
        ValueT mValue;
    };

    class LogLevel : public TagType<LogLevel, std::string>
    {
    public:
        static constexpr char key[] = "log_level";
        LogLevel(std::string const &value)
            : TagType(value)
        {
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
    struct FilterClause
    {
        std::vector<std::unique_ptr<Tag>> normalLiterals;
        std::vector<std::unique_ptr<Tag>> invertedLiterals;
    };
    inline std::map<int, FilterClause> &getFilterClauses()
    {
        static std::map<int, FilterClause> clauses;
        return clauses;
    }
    inline int createFilterClause()
    {
        static int currentId = 0;
        ++currentId;
        auto &clauses = getFilterClauses();
        clauses[currentId] = FilterClause();
        return currentId;
    }

    inline void addFilterLiteral(int filterId,
                                 Tag const &tag,
                                 bool normal = true)
    {
        auto &clauses = getFilterClauses();
        if (clauses.contains(filterId))
        {
            if (normal)
            {
                clauses[filterId].normalLiterals.push_back(
                    tag.clone());
            }
            else
            {
                clauses[filterId].invertedLiterals.push_back(
                    tag.clone());
            }
        }
    }

    inline void clearFilterClause(int filterId)
    {
        auto &clauses = getFilterClauses();
        clauses.erase(filterId);
    }

    class LogStream : public std::fstream
    {
    public:
        LogStream(std::string const &filename,
                  std::ios_base::openmode mode = ios_base::app)
            : std::fstream(filename, mode)
        {
        }
        LogStream(LogStream const &other) = delete;
        LogStream(LogStream &&other)
            : std::fstream(std::move(other))
        {
        }
        ~LogStream()
        {
            *this << std::endl;
        }

        LogStream &operator=(LogStream const &rhs) = delete;
        LogStream &operator=(LogStream &&rhs) = delete;
    };
    inline std::fstream log(std::vector<Tag const *> tags = {})
    {
        const auto now = std::chrono::system_clock::now();
        std::time_t const tmNow = std::chrono::system_clock::to_time_t(now);
        auto const ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        LogStream ls("application.log");
        ls << std::put_time(std::gmtime(&tmNow), "%Y-%m-%dT%H:%M:%S.") << std::setw(3) << std::setfill('0') << std::to_string(ms.count()) << " ";
        for (auto const &defaultTag : getDefaultTags())
        {
            if (std::find_if(tags.begin(), tags.end(),
                             [&defaultTag](auto const &tag)
                             {
                                 return defaultTag.first == tag->key();
                             }) == tags.end())
            {
                ls << " " << defaultTag.second->text();
            }
        }
        for (auto const &tag : tags)
        {
            ls << " " << tag->text();
        }
        ls << " ";
        return ls;
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
    inline std::fstream &operator<<(std::fstream &&stream, Tag const &tag)
    {
        stream << to_string(tag);
        return stream;
    }
}
