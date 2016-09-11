#include "Debug.hpp"

static std::ostream* theLogStreams[16];
static Logger theLogger = {theLogStreams, 0};

Logger Log()
{
    return theLogger;
}

void AddLogStream(std::ostream& stream)
{
    theLogger.data[theLogger.count++] = &stream;
}

void RemoveAllLogStreams()
{
    theLogger.count = 0;
}

void FlushLog()
{
    for (auto stream : theLogger) stream->flush();
}
