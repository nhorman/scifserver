/**
 * \file client.c client handler functions 
 */
#include <sserver.h>
#include <client-private.h>
#include <messages.pb-c.h>


static int read_data_from_ssl(struct client *c, void *buf, size_t *len)
{
	int rc;
retry_write:
	memset(buf, 0, *len);
	rc = SSL_get_error(c->ssl, SSL_read_ex(c->ssl, buf, *len, len));
	switch (rc) {
		case SSL_ERROR_NONE:
			break;
		case SSL_ERROR_SSL:
			g_warning("Unrecoverable ssl error\n");
			delete_client(c->sd);
			*len = 0;
			break;
		case SSL_ERROR_ZERO_RETURN:
			g_info("Peer has closed connection, deleting client\n");
			delete_client(c->sd);
			break;
		case SSL_ERROR_WANT_READ:
			g_warning("Read error on write, this doesn't make sense!\n");
			break;
		case SSL_ERROR_WANT_WRITE:
			g_warning("Write failed, retrying\n");
			goto retry_write;
			break;
		case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_WANT_ACCEPT:
			g_warning("Trying to work unconnected socket\n");
			delete_client(c->sd);
			break;
		case SSL_ERROR_SYSCALL:
			if (errno == -EPIPE) {
				g_warning("Remote client closed connection\n");
				delete_client(c->sd);
			} else {
				g_warning("syscall error: %s\n", strerror(errno));
			}
			break;
		default:
			g_warning("Unhandled return code %d\n", rc);
			delete_client(c->sd);
			break;
	}

	return rc;
}

static int write_data_to_ssl(struct client *c, void *buf, uint32_t len)
{
	int rc;
retry_write:
	rc = SSL_get_error(c->ssl, SSL_write(c->ssl, buf, len));
	switch (rc) {
		case SSL_ERROR_NONE:
			break;
                case SSL_ERROR_SSL:
                        g_warning("Unrecoverable ssl error\n");
                        delete_client(c->sd);
                        break;
		case SSL_ERROR_ZERO_RETURN:
			g_info("Peer has closed connection, deleting client\n");
			delete_client(c->sd);
			break;
		case SSL_ERROR_WANT_READ:
			g_warning("Read error on write, this doesn't make sense!\n");
			break;
		case SSL_ERROR_WANT_WRITE:
			g_warning("Write failed, retrying\n");
			goto retry_write;
			break;
		case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_WANT_ACCEPT:
			g_warning("Trying to work unconnected socket\n");
			delete_client(c->sd);
			break;
		case SSL_ERROR_SYSCALL:
			if (errno == -EPIPE) {
				g_warning("Remote client closed connection\n");
				delete_client(c->sd);
			} else {
				g_warning("syscall error: %s\n", strerror(errno));
			}
			break;
		default:
			g_warning("Unhandled return code %d\n", rc);
			delete_client(c->sd);
			break;
	}

	return rc;
}

static int handle_client_response(struct client *c){
	uint8_t buf[1024];
	size_t len = 1024;

	g_info("Got client response\n");
	read_data_from_ssl(c, buf, &len);
	g_info("Got %lu data bytes from ssl\n", len);
	return 0;
}

int start_client_tls(struct client *c)
{
	IdentifyRequest msg = IDENTIFY_REQUEST__INIT;
	uint32_t len;
	void *buf;

	g_info("Establishing SSL connection for new client\n");
	c->ssl =  SSL_new(c->ctx);
	SSL_set_fd(c->ssl, c->sd);
	if (SSL_accept(c->ssl) <= 0) {
		g_warning("Unable to establish ssh session\n");
		ERR_print_errors_fp(stderr);
		delete_client(c->sd);
		goto out;
	}

	/* Send the identify request to the client */
	msg.nonce = "The Quick Brown Fox Jumped Over The Lazy Dog";
	len = identify_request__get_packed_size(&msg);
	buf = malloc(len);
	identify_request__pack(&msg, buf);
	g_info("Requesting Client user Identity\n");

	c->handler = handle_client_response;
	write_data_to_ssl(c, buf, len);

	free(buf);
out:
	return 0;
}

