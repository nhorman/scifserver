/** @file
 *
 * \brief log api definition.
 *
 * This file defines the loggin interface of the microedge project
 *
 * The api is fairly strightforward, and consists of 5 functions:
 * log_open  Start the selected logging backend
 * log_close  Stop the selected logging backend
 * write_logmsg  Send a formatted log message to the opened log backend
 * select_logger  Select which logging backend to use
 * print_logger  display the set of registered log backends
 * fetch_log  fetch lines from the existing log file
 *
 *
 */

#ifndef __LOG_H__
#define __LOG_H__
#include <sserver.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef LOG_EMERG
#define LOG_EMERG       0       /* system is unusable */
#endif

#ifdef LOG_ALERT
#define LOG_ALERT       1       /* action must be taken immediately */
#endif

#ifdef LOG_CRIT
#define LOG_CRIT        2       /* critical conditions */
#endif

#ifndef LOG_ERR
#define LOG_ERR         3       /* error conditions */
#endif

#ifndef LOG_WARNING
#define LOG_WARNING     4       /* warning conditions */
#endif

#ifndef LOG_NOTICE
#define LOG_NOTICE      5       /* normal but significant condition */
#endif

#ifndef LOG_INFO
#define LOG_INFO        6       /* informational */
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG       7       /* debug-level messages */
#endif

#define LOGMSG(prio, fmt, ...) do { fprintf(stderr, "%s|%s|%d|" fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); } while (0)
#endif
