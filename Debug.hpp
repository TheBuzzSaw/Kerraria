#ifndef Debug_hpp
#define Debug_hpp

#include "Span.hpp"
#include <iostream>
#include <utility>

typedef Span<std::ostream*> Logger;

template<class T> Logger operator<<(Logger streams, T&& value)
{
    for (auto stream : streams) *stream << std::forward<T>(value);
    return streams;
}

Logger Log();

void AddLogStream(std::ostream& stream);
void RemoveAllLogStreams();
void FlushLog();

#endif
