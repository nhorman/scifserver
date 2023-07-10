#include <sserver.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static GSource *listen_socket_source = NULL;

int setup_server_listening_socket(GMainLoop *loop)
{
	int rc = -ENOMEM;
	int sd;
	struct sockaddr_in addr;
	
	/* start by allocating a socket */
	sd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

	if (sd < 0) {
		rc = errno;
		LOGMSG(G_LOG_LEVEL_ERROR, "Unable to create socket\n");
		goto out;
	}

	addr.sin_family = AF_INET6;
	addr.sin_port = 4443;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		rc = errno;
		LOGMSG(G_LOG_LEVEL_ERROR, "Unable to bind socket\n");
		goto out_close;
	}

	if (listen(s, 10)) {
		rc = errno;
		LOGMSG(G_LOG_LEVEL_ERROR, "Unable to set socket ot listen\n");
		goto out_close;
	}

	listen_socket_source = g_unix_fd_source_new(sd, G_IO_IN);
	if (!listen_socket_source) {
		rc = -EFAULT;
		LOGMSG(G_LOG_LEVEL_ERROR, "Unable to create glib source for listen socket\n");
		goto out_close;
	}
	g_source_set_callback(listen_socket_source, NULL, NULL);
	g_source_attach(listen_socket_source, NULL);
	rc = 0;
out:
	return rc;
out_close:
	close(sd);
	goto out;
}


