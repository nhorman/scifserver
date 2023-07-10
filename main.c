#include <sserver.h>

int main(int argc, char *argv[])
{
	GMainLoop *mainloop = NULL;
	int rc = 0;

	g_log_set_writer_func (g_log_writer_journald, NULL, NULL);

	LOGMSG(G_LOG_LEVEL_INFO, "Starting scifserver\n");

	mainloop = g_main_loop_new(NULL, false);
	if (!mainloop) {
		LOGMSG(G_LOG_LEVEL_ERROR, "Unable to create main loop\n");
		rc = -ENOMEM;
		goto out;
	}
	g_main_loop_run(mainloop);

	rc = 0;
out:
	return rc;
}
