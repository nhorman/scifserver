#include <sserver.h>

int main(int argc, char __unused *argv[])
{
	int rc = 0;
	LOGMSG(LOG_INFO, "Starting SCIF server\n");

	if (config_components()) {
		LOGMSG(LOG_ERR, "Failed to config subsystems, shutting down\n");
		rc = 1;
		goto out;
	}

	if (config_read_config(DEFAULT_CONFIG_PATH)) {
		LOGMSG(LOG_ERR, "Unable to read config file\n");
	}

out:
	return rc;
}
