#include "stdafx.h"
#include "..\..\..\include\gepimpl\subsystems\logging.h"
#include <cstdarg>
#include <iostream>
#include <iomanip>
#include <cstdarg>


void gep::Logging::logMessage(GEP_PRINTF_FORMAT_STRING const char * fmt, ...)
{
	printMessage(LogChannel::message, fmt);
}

void gep::Logging::logWarning(GEP_PRINTF_FORMAT_STRING const char * fmt, ...)
{
	printMessage(LogChannel::warning, fmt);
}

void gep::Logging::logError(GEP_PRINTF_FORMAT_STRING const char * fmt, ...)
{
	printMessage(LogChannel::error, fmt);
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

void gep::Logging::printMessage(LogChannel channel, const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int len = _vscprintf(fmt, args) + 1;
	char* msg = new char[len];
	vsprintf_s(msg, len, fmt, args);

	for (int i = 0; i < MAX_SINK_OBJECTS; i++)
	{
		m_pSinkObjects[i]->take(channel , msg);
	}

	va_end(args);
	delete[] msg;
}

#include <iostream>
gep::Logging::Logging()
{
	memset(m_pSinkObjects, 0, sizeof(m_pSinkObjects));
}

gep::Logging::~Logging()
{
	for (int i = 0; i < MAX_SINK_OBJECTS; i++)
	{
		delete m_pSinkObjects[i];
		m_pSinkObjects[i] = nullptr;
	}
}

void gep::ConsoleLogSink::take(LogChannel channel, const char * msg)
{
	//https://en.cppreference.com/w/cpp/io/manip/put_time for the time format
	std::time_t time = std::time(nullptr);
	std::cout << std::put_time(std::localtime(&time), "%b %F %T") << ": " << ILogSink::logChannelChar(channel) << ": " << msg << "\n";
}

void gep::FileLogSink::take(LogChannel channel, const char * msg)
{
	std::ofstream logStream;
	logStream.open("SystemLogFile.log", logStream.app);

	//https://en.cppreference.com/w/cpp/io/manip/put_time for the time format
	std::time_t time = std::time(nullptr);
	logStream <<std::put_time(std::localtime(&time), "%b %F %T") <<": "<< ILogSink::logChannelChar(channel) << ": " << msg << "\n";
	logStream.flush();
	logStream.close();
}
