/**
 * \file client.c client handler functions 
 */
#include <sserver.h>
#include <client-private.h>
#include <messages.pb-c.h>

static int handle_client_response(client *c){
	g_info("Got client response\n");
}

int start_client_tls(struct client *c)
{
	identify_request msg = IDENTIFY_REQUEST_INIT;
	uint32_t len;
	void *buf;

	g_info("Establishing SSL connection for new client\n");
	c->ssl =  SSL_new(c->ctx);
	SSL_set_fd(c->ssl, c->sd);
	if (SSL_accept(c->ssl) <= 0) {
		g_warning("Unable to establish ssh session\n");
		delete_client(c->sd);
	}

	/* Send the identify request to the client */
	msg.nonce = "The Quick Brown Fox Jumped Over The Lazy Dog";
	len = identify_request__get_packetd_size(&msg);
	buf = malloc(len);
	identify_request__pack(&msg, buf);
	g_info("Requesting Client user Identity\n");
	SSL_write(c->ssl, buf, len);
	free(buf);
	c->handler = handle_client_response;
	return 0;
}

