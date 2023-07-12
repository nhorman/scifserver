#include <sserver.h>
#include <stdio.h>

static void log_handler(const gchar *domain, GLogLevelFlags log_level, const gchar *message, gpointer data)
{
	fprintf(stderr, "%s: %s", domain, message);
}

int main(int argc, char *argv[])
{
	GMainLoop *mainloop = NULL;
	int rc = 0;

	g_log_set_default_handler(log_handler, NULL);

	g_info("Starting scifserver\n");

	mainloop = g_main_loop_new(NULL, false);
	if (!mainloop) {
		g_error("Unable to create main loop\n");
		rc = -ENOMEM;
		goto out;
	}

	rc = setup_client_handling(-1);
	if (rc) {
		g_error("Unable to setup client handling\n");
	}
	rc = setup_server_listening_socket(mainloop);
	if (rc) {
		g_error("Unable to create server socket\n");
		goto out;
	}

	g_main_loop_run(mainloop);

	rc = cleanup_client_handling();
	if (rc) {
		g_warning("Unable to cleanup client handling\n");
	}
	rc = shutdown_listening_socket();
	if (rc) {
		g_warning("Unable to shutdown listening socket\n");
	}

	rc = 0;
out:
	return rc;
}
