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

out:
	return rc;
}
