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
#include <component.h>
#include <log.h>

/**
 * @brief tag to indicate a variable is unused 
 */
#define __unused  __attribute__((unused))

/**
 * \brief macro to compute the number of elements in an array at compile time
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif

