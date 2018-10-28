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

		// Inherited via ILogging
		virtual void logMessage(GEP_PRINTF_FORMAT_STRING const char * fmt, ...) override;
		virtual void logWarning(GEP_PRINTF_FORMAT_STRING const char * fmt, ...) override;
		virtual void logError(GEP_PRINTF_FORMAT_STRING const char * fmt, ...) override;
		virtual void registerSink(ILogSink * pSink) override;
		virtual void deregisterSink(ILogSink * pSink) override;

		Logging();
		~Logging();
	};

    class ConsoleLogSink : public ILogSink
    {
		// Inherited via ILogSink
		virtual void take(LogChannel channel, const char * msg) override;
		
	public:
		ConsoleLogSink();
		~ConsoleLogSink();
	};

	class FileLogSink : public ILogSink
    {
		std::ofstream logStream;
		//Inherited via ILogSink
		virtual void take(LogChannel channel, const char * msg) override;
	
	public:
		FileLogSink();
		~FileLogSink();
	};

	
}
