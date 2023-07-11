#include <sserver.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <glib-unix.h>

static GSource *listen_socket_source = NULL;

static gboolean accept_new_socket(gpointer arg)
{
	int sd = (int)arg;
	int newcfg;
	struct sockaddr_in newsock;

	g_info("Accpeting new connection\n");
	newcfd = accept(sd, &newsock, sizeof(struct sockaddr_in));
	
	return G_SOURCE_CONTINUE;
}

int setup_server_listening_socket(GMainLoop *loop)
{
	int rc = -ENOMEM;
	int sd;
	struct sockaddr_in addr;
	
	/* start by allocating a socket */
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sd < 0) {
		rc = errno;
		g_error("Unable to create socket\n");
		goto out;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(4443);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		rc = errno;
		g_warning("Unable to bind socket: %s\n", strerror(errno));
		goto out_close;
	}

	if (listen(sd, 1)) {
		rc = errno;
		g_warning("Unable to listen on socket\n");
		goto out_close;
	}

	listen_socket_source = g_unix_fd_source_new(sd, G_IO_IN);
	if (!listen_socket_source) {
		rc = -EFAULT;
		g_warning("Unable to create source\n");
		goto out_close;
	}
	g_source_set_callback(listen_socket_source, accept_new_socket, sd, NULL);
	g_source_attach(listen_socket_source, g_main_loop_get_context(loop));
	rc = 0;
out:
	return rc;
out_close:
	close(sd);
	goto out;
}


