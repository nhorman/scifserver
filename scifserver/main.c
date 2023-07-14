#include <sserver.h>
#include <stdio.h>
#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#else
#error NEED A GETOPT_LONG IMPLEMENTATION
#endif


static char *keyfile = NULL;
static char *certfile = NULL;

#ifdef HAVE_GETOPT_LONG
struct option lopts[] = {
        { "keyfile",    0, NULL, 'k' },
	{ "certfile",   0, NULL, 'c' },
	{ "help",       0, NULL, 'h' },
        { 0,               0, 0,    0   }
};
#endif

static const char *shortopts = "k:c:h";

static void print_usage(char *argv[])
{
	g_info("Usage: %s <-k|--keyfile <keyfile.pem>> <-c|--certfile> <certfile.pem>>", argv[0]);
}

static int parse_args(int argc, char *argv[])
{
	int opt;

#ifdef HAVE_GETOPT_LONG
	int longind;
        while ((opt = getopt_long(argc, argv,
                                  shortopts,
                                  lopts, &longind)) != -1) {
#else
        while ((opt = getopt(argc, argv,
                             shortopts)) != -1) {
#endif
		switch(opt) {
			case 'k':
				keyfile = optarg;
				break;
			case 'c':
				certfile = optarg;
				break;
			case 'h':
				print_usage(argv);
				exit(0);
			default:
				g_info("Unknown option %c\n", opt);
				exit(0);

		}
	}

	return 0;
}


static void log_handler(const gchar *domain, GLogLevelFlags log_level, const gchar *message, gpointer data)
{
	fprintf(stderr, "%s: %s", domain, message);
}

int main(int argc, char *argv[])
{
	GMainLoop *mainloop = NULL;
	int rc = 0;

	g_log_set_default_handler(log_handler, NULL);
	signal(SIGPIPE, SIG_IGN);

	parse_args(argc, argv);

	if (!keyfile) {
		g_warning("A server key file is required\n");
		goto out;
	}
	if (!certfile) {
		g_warning("A server certificate is required\n");
		goto out;
	}

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
	rc = setup_server_listening_socket(mainloop, keyfile, certfile);
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
