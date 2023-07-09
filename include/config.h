/** @file
 *
 * \brief configuration subsystem api 
 *
 * This file defines the config subsystem api
 *
 *
 *
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <sserver.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * \brief get a configuration item
 * fetches a config value from the config tree 
 * using JSONPath syntax
 * \param path - JsonPath string of item to fetch
 * \param val - pointer to pointer to returned value
 * \returns 0 on success
 * \returns -ENOENT on path not found
 * \return  -EINVAL on all other errors
 */
int config_get_item(const char *path, void **val);

/**
 * \brief set a cmdline value
 * simple cmd line db interface
 * \param name - name to set
 * \param value - pointer to pointer value to store for name
 * \returns 0 on success
 * \returns 1 on sucess when value was replaced
 * \returns < 0 on error
 */
int config_set_cmdline_value(const char *name, void **value);

/**
 * \brief get a cmdline value
 * simple cmd line db interface
 * \param name - name to get
 * \param value - pointer to pointer value to fetch for name
 * \returns 0 on success
 * \returns < 0 on error
 */
int config_get_cmdline_value(const char *name, void **value);

/**
 * \brief read configuration file
 * reads in a json formatted configuration file
 * \param path - path to config file
 * \returns 0 on successful read
 * \returns < 0 on failure
 */
int config_read_config(const char *path);

#endif
