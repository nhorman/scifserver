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
#include <pthread.h>
#include <json-glib/json-glib.h>

static pthread_mutex_t config_lock = PTHREAD_MUTEX_INITIALIZER;
static JsonParser *configparser = NULL;

int config_get_item(const char __unused *path, void __unused **val)
{
	return 0;
}

int config_read_config(const char *path)
{
	GError *err = NULL;
	pthread_mutex_lock(&config_lock);
	if (configparser) {
		g_object_unref(configparser);
		configparser = NULL;
	}
	configparser = json_parser_new();
	if (!configparser) {
		LOGMSG(LOG_ERR, "Unable to allocate a new config parser\n");
		return -ENOMEM;
	}
	if (!json_parser_load_from_file(configparser, path, &err)) {
		LOGMSG(LOG_ERR, "Unable to load config file: %s\n", err->message);
		g_error_free(err);
		g_object_unref(configparser);
		configparser = NULL;
		return -EINVAL;
	}
	pthread_mutex_unlock(&config_lock);
	return 0;
}

static int configure_configuration()
{
	return 0;
}

static void shutdown_configuration()
{
	pthread_mutex_lock(&config_lock);
	if (configparser) {
		g_object_unref(configparser);
		configparser = NULL;
	}
	return; 
}

REGISTER_COMPONENT(configuration, configure_configuration, NULL, NULL, shutdown_configuration, COMPONENT_EARLY_INIT);
