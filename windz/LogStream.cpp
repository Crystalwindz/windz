//
// Created by crystalwind on 19-2-10.
//

#include "LogStream.h"
#include <assert.h>
#include <strings.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <limits>

namespace windz {

namespace {

const char digits[] = "9876543210123456789";
const char *zero = digits + 9;

const char digitsHex[] = "0123456789abcdef";

// From muduo
// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t Convert(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

size_t ConvertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

}  // namespace

template<typename T>
void LogStream::FormatInteger(T v) {
    if (buffer_.Avail() >= kMaxNumericSize) {
        size_t len = Convert(buffer_.cur(), v);
        buffer_.Add(len);
    }
}

LogStream &LogStream::operator<<(bool v) {
    buffer_.Append(v ? "1" : "0", 1);
    return *this;
}

LogStream &LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned short v) {
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream &LogStream::operator<<(int v) {
    FormatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned int v) {
    FormatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(long v) {
    FormatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long v) {
    FormatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(long long v) {
    FormatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long v) {
    FormatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
}

LogStream &LogStream::operator<<(double v) {
    if (buffer_.Avail() >= kMaxNumericSize) {
        int len = snprintf(buffer_.cur(), kMaxNumericSize, "%.12g", v);
        buffer_.Add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(long double v) {
    if (buffer_.Avail() >= kMaxNumericSize) {
        int len = snprintf(buffer_.cur(), kMaxNumericSize, "%.12Lg", v);
        buffer_.Add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(char v) {
    buffer_.Append(&v, 1);
    return *this;
}

LogStream &LogStream::operator<<(const char * v) {
    if (v) {
        buffer_.Append(v, strlen(v));
    } else {
        buffer_.Append("(null)", 6);
    }
    return *this;
}

LogStream &LogStream::operator<<(const unsigned char * v) {
    *this << reinterpret_cast<const char *>(v);
    return *this;
}

LogStream &LogStream::operator<<(const std::string & v) {
    buffer_.Append(v.c_str(), v.length());
    return *this;
}

LogStream &LogStream::operator<<(const void * p) {
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if (buffer_.Avail() >= kMaxNumericSize) {
        char *buf = buffer_.cur();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = ConvertHex(buf+2, v);
        buffer_.Add(len+2);
    }
    return *this;
}

}  // namespace windz
