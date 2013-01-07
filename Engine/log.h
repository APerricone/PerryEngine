#pragma once

#include <stdarg.h>

class ILog
{
	static ILog *s_pLog;
public:
	static void SetLog(ILog *i_pLog) { s_pLog = i_pLog; }

	static void Message(const char* i_pMessage, ... );
	static void Error(const char* i_pMessage, ... );
protected:
	virtual void InternalMessage(const char* i_pMessage, va_list args) = 0;
	virtual void InternalError(const char* i_pMessage, va_list args) = 0;
};

