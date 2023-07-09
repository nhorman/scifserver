/** @file
 *
 * \brief configuration component api 
 *
 * This file defines the config subsystem api
 *
 *
 *
 */

#include <sserver.h>

int config_get_item(const char __unused *path, void __unused **val)
{
	return 0;
}

int config_set_cmdline_value(const char __unused *name, void __unused **value)
{
	return 0;
}

int config_get_cmdline_value(const char __unused *name, void __unused **value)
{
	return 0;
}

int config_read_config(const char __unused *path)
{
	return 0;
}
