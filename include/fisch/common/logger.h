#pragma once
#include "logger/syslog/logger.h"
#include <log4cxx/logger.h>

#ifdef SYSLOG_THRESHOLD
#define _SYSLOG_THRESHOLD_PREDEFINED
#else
#ifdef FISCH_SYSLOG_THRESHOLD
#define SYSLOG_THRESHOLD FISCH_SYSLOG_THRESHOLD
#else
#define SYSLOG_THRESHOLD logger::LogPriority::INFO
#endif
#endif

#define FISCH_LOG_SYSLOG(level, message) LOGGER_OPEN_SYSLOG_CLOSE(level, "fisch", message)

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 0
#define FISCH_LOG_TRACE(logger, message) LOG4CXX_TRACE(logger, message)
#else
#define FISCH_LOG_TRACE(logger, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 1
#define FISCH_LOG_DEBUG(logger, message)                                                           \
	LOG4CXX_DEBUG(logger, message)                                                                 \
	FISCH_LOG_SYSLOG(DEBUG, message)
#else
#define FISCH_LOG_DEBUG(logger, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 2
#define FISCH_LOG_INFO(logger, message)                                                            \
	LOG4CXX_INFO(logger, message)                                                                  \
	FISCH_LOG_SYSLOG(INFO, message)
#else
#define FISCH_LOG_INFO(logger, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 3
#define FISCH_LOG_WARN(logger, message)                                                            \
	LOG4CXX_WARN(logger, message);                                                                 \
	FISCH_LOG_SYSLOG(WARN, message)
#else
#define FISCH_LOG_WARN(logger, message) FISCH_LOG_SYSLOG(LOG_WARNING, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 4
#define FISCH_LOG_ERROR(logger, message)                                                           \
	LOG4CXX_ERROR(logger, message);                                                                \
	FISCH_LOG_SYSLOG(ERROR, message)
#else
#define FISCH_LOG_ERROR(logger, message) FISCH_LOG_SYSLOG(LOG_ERR, message)
#endif

#if !defined(FISCH_LOG_THRESHOLD) || FISCH_LOG_THRESHOLD <= 5
#define FISCH_LOG_FATAL(logger, message)                                                           \
	LOG4CXX_FATAL(logger, message);                                                                \
	FISCH_LOG_SYSLOG(CRIT, message)
#else
#define FISCH_LOG_FATAL(logger, message) FISCH_LOG_SYSLOG(LOG_CRIT, message)
#endif

#ifdef _SYSLOG_THRESHOLD_PREDEFINED
#undef _SYSLOG_THRESHOLD_PREDEFINED
#else
#undef SYSLOG_THRESHOLD
#endif