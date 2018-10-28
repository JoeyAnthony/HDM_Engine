#pragma once
#include "gep/interfaces/subsystem.h"
#include <map>


namespace gep
{
    enum class LogChannel
    {
        message,
        warning,
        error
    };

    class ILogSink
    {
    public:
        virtual ~ILogSink() {}
        virtual void take(LogChannel channel, const char* msg) = 0;

		static inline const char* logChannelChar(LogChannel channel)
		{
			const std::map<LogChannel, const char*> enumStrings{
				{LogChannel::message, "Message"},
				{LogChannel::warning, "Warning"},
				{LogChannel::error, "Error"}
			};

			auto found = enumStrings.find(channel);
			return found == enumStrings.end() ? "Out of range" : found->second;
		}
    };

    class ILogging
    {
    public:
        virtual ~ILogging() {}
		/// \brief log a regular message
        virtual void logMessage(GEP_PRINTF_FORMAT_STRING const char* fmt, ...) = 0;
		/// \brief log a warning
        virtual void logWarning(GEP_PRINTF_FORMAT_STRING const char* fmt, ...) = 0;
		/// \brief log an error
        virtual void logError(GEP_PRINTF_FORMAT_STRING const char* fmt, ...) = 0;

        virtual void registerSink(ILogSink* pSink) = 0;
        virtual void deregisterSink(ILogSink* pSink) = 0;

    };
}
