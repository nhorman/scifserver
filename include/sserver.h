/**
 * \file sserver.h
 * \brief top level include file for scifserver 
 *
 * This file should include any common headers/defines/etc that all files in this 
 * program need
 *
 */

#ifndef __SSERVER_H__
#define __SSERVER_H__
#include <ss-config.h>
#include <stdbool.h>
#include <glib.h>

#define LOGMSG(prio, fmt, args...) g_log_structured(G_LOG_DOMAIN, prio, "MESSAGE", fmt, ## args)
#endif

