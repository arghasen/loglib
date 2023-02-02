#pragma once

#include "../Log.h"
inline loglib::LogLevel error("error");
inline loglib::LogLevel info("info");
inline loglib::LogLevel debug("debug");

class Color : public loglib::StringTagType<Color>
{
public:
    static constexpr char key[] = "color";
    Color (std::string const & value,
        loglib::TagOperation operation =
            loglib::TagOperation::None)
    : StringTagType(value, operation)
    { }
};

inline Color red("red");
inline Color green("green");
inline Color blue("blue");

class Size : public loglib::StringTagType<Size>
{
public:
    static constexpr char key[] = "size";
    Size (std::string const & value,
        loglib::TagOperation operation =
            loglib::TagOperation::None)
    : StringTagType(value, operation)
    { }
};

inline Size small("small");
inline Size medium("medium");
inline Size large("large");

class Count : public loglib::IntTagType<Count>
{
public:
    static constexpr char key[] = "count";
    Count (int value,
        loglib::TagOperation operation =
            loglib::TagOperation::None)
    : IntTagType(value, operation)
    { }
};

class Identity : public loglib::LongLongTagType<Identity>
{
public:
    static constexpr char key[] = "id";
    Identity (long long value,
        loglib::TagOperation operation =
            loglib::TagOperation::None)
    : LongLongTagType(value, operation)
    { }
};

class Scale : public loglib::DoubleTagType<Scale>
{
public:
    static constexpr char key[] = "scale";
    Scale (double value,
        loglib::TagOperation operation =
            loglib::TagOperation::None)
    : DoubleTagType(value, operation)
    { }
};

class CacheHit : public loglib::BoolTagType<CacheHit>
{
public:
    static constexpr char key[] = "cache_hit";
    CacheHit (bool value,
        loglib::TagOperation operation =
            loglib::TagOperation::None)
    : BoolTagType(value, operation)
    { }
};

inline CacheHit cacheHit(true);
inline CacheHit cacheMiss(false);