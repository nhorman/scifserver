/**
 * \file client.c client handler functions 
 */
#include <sserver.h>
#include <client-private.h>


int start_client_tls(struct client *c)
{
	g_info("Establishing SSL connection for new client\n");
	c->ssl =  SSL_new(c->ctx);
	SSL_set_fd(c->ssl, c->sd);
	if (SSL_accept(c->ssl) <= 0) {
		g_warning("Unable to establish ssh session\n");
		delete_client(c->sd);
	}

	return 0;
}

