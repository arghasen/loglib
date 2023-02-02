#pragma once

#include "../Log.h"
inline loglib::LogLevel error("error");
inline loglib::LogLevel info("info");
inline loglib::LogLevel debug("debug");

class Color : public loglib::Tag
{
public:
    Color(std::string const &text)
        : Tag("color", text)
    {
    }

    std::unique_ptr<Tag> clone() const override
    {
        return std::unique_ptr<Tag>(
            new Color(*this));
    }
};

inline Color red("red");
inline Color green("green");
inline Color blue("blue");