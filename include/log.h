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

/*! \def LOGMSG(prio, fmt, args...)
 * \brief macro that should be used when issuing all log messages
 * \param prio  the priority of the message
 * \param fmt  the format string of the message
 * \param args  variadic arguments to message format string
 */
#define LOGMSG(prio, fmt, args ...) do { write_logmsg(prio, __FILE__, __FUNCTION__, __LINE__, fmt, ## args); } while (0)

int log_open();


/*!
 * \brief emits a formatted log message to the logger backend
 *
 * \param[in] prio integer priority - taken from the LOG_* values in syslog.h
 *  - LOG_EMERG      system is unusable
 *  - LOG_ALERT      action must be taken immediately
 *  - LOG_CRIT       critical conditions
 *  - LOG_ERR        error conditions
 *  - LOG_WARNING    warning conditions
 *  - LOG_NOTICE     normal, but significant, condition
 *  - LOG_INFO       informational message
 *  - LOG_DEBUG      debug-level message
 * \param[in] file the C file name the message was called from
 * \param[in] func the function that the message was called form
 * \param[in] line the line number that the message was called from
 * \param[in] fmt the format of the message
 * \param[in] ... variadic arguments to fmt
 */
void write_logmsg(int prio, const char *file, const char *func, int line, const char *fmt, ...) \
	__attribute__ ((format(printf, 5, 6)));


void log_close();

/**
 * \brief Select a logger.
 *
 * This method selects a logger for use by the system.
 *
 * \param[in] name Name of logger to select.
 * \retval 0 Success.
 * \retval nonzero Failure - \a name not found.
 */
int select_logger(const char *name);

/**
 * \brief Get name of selected logger.
 *
 * \warning The name returned by this function is only a snapshot and may not
 * be accurate after this function exits; another thread may select a different
 * logger immediately after the name is returned.
 *
 * \return Name of selected logger, or NULL if none selected.
 */
const char *get_selected_logger();

/**
 * \brief Print all registered loggers to stdout.
 */
void print_loggers();

/**
 * \brief attribute to mark a function as registering a log backend
 */
#define __loggerreg __attribute__((constructor))

/**
 * @brief A structure to define a logging backend
 */
struct log_register_info {
	char *                          name;                                                           /**< The name of the logging backend */
	int                             (*open_log)();                                                  /**< Function pointer to the backend open function */
	void                            (*close_log)();                                                 /**< Function pointer to the backend close function */
	void                            (*emit_log)(int prio, const char *msg);                         /**< Function pointer to the backend emit function */
	struct log_register_info *      next;                                                           /**< Internal list maintenence, set to NULL */
	bool                            in_use;                                                         /**< internal state tracker */
};

/**
 * \fn void register_logger()
 * \brief register a log backend based on the log_register_info struct
 * \param ops - log_register_info struct to define the logger backend
 */
void register_logger(struct log_register_info *ops);

/**
 * \def REGISTER_LOG_BACKEND(name, ofn, cfn, efn)
 * \brief macro to make registration easy at start time
 * instancing this macro in a file will automaticaly register the defined
 * backend at startup via a constructor
 * \param ofn - the log backend open function
 * \param cfn - the log backend close function
 * \param efn - the log backend emit function
 */
#define REGISTER_LOG_BACKEND(name, ofn, cfn, efn) \
        struct log_register_info __ ## name ## _info = { \
                #name, \
                ofn, \
                cfn, \
                efn, \
                NULL, \
                false \
        }; \
        void __loggerreg register_ ## name ## _logger() { \
                register_logger(&__ ## name ## _info); \
        } \

#endif
