#pragma once
//#include "gep/common.h"
#include "gep/interfaces/logging.h"

namespace gep
{
	#define MAX_SINK_OBJECTS 2
    class Logging : public ILogging
    {
	private:
		ILogSink* m_pSinkObjects[MAX_SINK_OBJECTS];
	public:

		/// Inherited via ILogging
		virtual void logMessage(GEP_PRINTF_FORMAT_STRING const char * fmt, ...) override;
		virtual void logWarning(GEP_PRINTF_FORMAT_STRING const char * fmt, ...) override;
		virtual void logError(GEP_PRINTF_FORMAT_STRING const char * fmt, ...) override;
		virtual void registerSink(ILogSink * pSink) override;
		virtual void deregisterSink(ILogSink * pSink) override;

		///\brief prints the message in all registered sinks with the given logchannel
		void printMessage(LogChannel channel, const char * fmt, ...);

		Logging();
		~Logging();
	};

    class ConsoleLogSink : public ILogSink
    {
		// Inherited via ILogSink
		virtual void take(LogChannel channel, const char * msg) override;
	};

	class FileLogSink : public ILogSink
    {
		const char* filename = "SystemLogFile.log";
		//Inherited via ILogSink
		virtual void take(LogChannel channel, const char * msg) override;
	};

	
}
