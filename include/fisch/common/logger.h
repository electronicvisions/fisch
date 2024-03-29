#pragma once
#include <log4cxx/logger.h>

extern "C"
{
#include <syslog.h>
}

#define FISCH_LOG_SYSLOG(level, message)                                                           \
	{                                                                                              \
		std::stringstream msg;                                                                     \
		msg << message;                                                                            \
		openlog(NULL, (LOG_NOWAIT | LOG_NDELAY | LOG_PID), LOG_USER);                              \
		syslog(level, "(%s) %s", #level, msg.str().c_str());                                       \
		closelog();                                                                                \
	}

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 0
#define FISCH_LOG_TRACE(logger, message) LOG4CXX_TRACE(logger, message)
#else
#define FISCH_LOG_TRACE(logger, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 1
#define FISCH_LOG_DEBUG(logger, message) LOG4CXX_DEBUG(logger, message)
#else
#define FISCH_LOG_DEBUG(logger, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 2
#define FISCH_LOG_INFO(logger, message) LOG4CXX_INFO(logger, message)
#else
#define FISCH_LOG_INFO(logger, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 3
#define FISCH_LOG_WARN(logger, message)                                                            \
	LOG4CXX_WARN(logger, message);                                                                 \
	FISCH_LOG_SYSLOG(LOG_WARNING, message)
#else
#define FISCH_LOG_WARN(logger, message) FISCH_LOG_SYSLOG(LOG_WARNING, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 4
#define FISCH_LOG_ERROR(logger, message)                                                           \
	LOG4CXX_ERROR(logger, message);                                                                \
	FISCH_LOG_SYSLOG(LOG_ERR, message)
#else
#define FISCH_LOG_ERROR(logger, message) FISCH_LOG_SYSLOG(LOG_ERR, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 5
#define FISCH_LOG_FATAL(logger, message)                                                           \
	LOG4CXX_FATAL(logger, message);                                                                \
	FISCH_LOG_SYSLOG(LOG_CRIT, message)
#else
#define FISCH_LOG_FATAL(logger, message) FISCH_LOG_SYSLOG(LOG_CRIT, message)
#endif
