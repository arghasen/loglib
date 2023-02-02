#pragma once

#include "../Log.h"
inline loglib::LogLevel error("error");
inline loglib::LogLevel info("info");
inline loglib::LogLevel debug("debug");

class Color : public loglib::TagType<Color, std::string>
{
public:
    static constexpr char key[] = "color";
    Color (std::string const & value)
    : TagType(value)
    { }
};

inline Color red("red");
inline Color green("green");
inline Color blue("blue");

class Size : public loglib::TagType<Size, std::string>
{
public:
    static constexpr char key[] = "size";
    Size (std::string const & value)
    : TagType(value)
    { }
};

inline Size small("small");
inline Size medium("medium");
inline Size large("large");

class Count : public loglib::TagType<Count, int>
{
public:
    static constexpr char key[] = "count";
    Count (int value)
    : TagType(value)
    { }
};

class Identity : public loglib::TagType<Identity, long long>
{
public:
    static constexpr char key[] = "id";
    Identity (long long value)
    : TagType(value)
    { }
};

class Scale : public loglib::TagType<Scale, double>
{
public:
    static constexpr char key[] = "scale";
    Scale (double value)
    : TagType(value)
    { }
};

class CacheHit : public loglib::TagType<CacheHit, bool>
{
public:
    static constexpr char key[] = "cache_hit";
    CacheHit (bool value)
    : TagType(value)
    { }
};

inline CacheHit cacheHit(true);
inline CacheHit cacheMiss(false);