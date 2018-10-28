#include "stdafx.h"
#include "..\..\..\include\gepimpl\subsystems\logging.h"

void gep::Logging::logMessage(GEP_PRINTF_FORMAT_STRING const char * fmt, ...)
{
	for (int i = 0; i < MAX_SINK_OBJECTS; i++)
	{

	}
}

void gep::Logging::logWarning(GEP_PRINTF_FORMAT_STRING const char * fmt, ...)
{
}

void gep::Logging::logError(GEP_PRINTF_FORMAT_STRING const char * fmt, ...)
{
}

void gep::Logging::registerSink(ILogSink * pSink)
{
	for (int i = 0; i < MAX_SINK_OBJECTS; i++)
	{
		if (m_pSinkObjects[i] == nullptr)
		{
			m_pSinkObjects[i] = pSink;
			return;
		}
	}
}

void gep::Logging::deregisterSink(ILogSink * pSink)
{
	for (int i = 0; i < MAX_SINK_OBJECTS; i++)
	{
		if (pSink == m_pSinkObjects[i])
		{
			//TODO call deregister function in LogSink
			// so it knows it is deregistered
			m_pSinkObjects[i] = nullptr;
			return;
		}
	}
}

#include <iostream>
gep::Logging::Logging()
{
	memset(m_pSinkObjects, 0, sizeof(m_pSinkObjects));

	//size tests
	//std::cout << "arr " << &m_pSinkObjects << " __size: " << sizeof(m_pSinkObjects) <<"\n";
	//std::cout << "index 0 " <<&m_pSinkObjects[0] << " __size: " << sizeof(m_pSinkObjects[0])<< "\n";
}

gep::Logging::~Logging()
{
	for (int i = 0; i < MAX_SINK_OBJECTS; i++)
	{
		delete m_pSinkObjects[i];
		m_pSinkObjects[i] = nullptr;
	}
	//TODO test if it deletes all pointers in the array correctly
}

gep::ConsoleLogSink::ConsoleLogSink()
{
}

gep::ConsoleLogSink::~ConsoleLogSink()
{
}

void gep::ConsoleLogSink::take(LogChannel channel, const char * msg)
{
	std::cout << ILogSink::logChannelChar(channel) << ": " << msg << "\n";
}

gep::FileLogSink::FileLogSink()
{
	logStream.open("SystemLogFile.log", logStream.app);
}

gep::FileLogSink::~FileLogSink()
{
	logStream.close();
}

void gep::FileLogSink::take(LogChannel channel, const char * msg)
{
	logStream << ILogSink::logChannelChar(channel) << ": " << msg << "\n";
}
