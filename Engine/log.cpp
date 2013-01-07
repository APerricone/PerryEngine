#include "log.h"

ILog *ILog::s_pLog = 0;

void ILog::Message(const char* i_pMessage, ... )
{
	if(s_pLog)
	{
		va_list args;
		va_start(args,i_pMessage);
		s_pLog->InternalMessage(i_pMessage,args);
		va_end(args);
	}
}

void ILog::Error(const char* i_pMessage, ... )
{
	if(s_pLog)
	{
		va_list args;
		va_start(args,i_pMessage);
		s_pLog->InternalError(i_pMessage,args);
		va_end(args);
	}
}
