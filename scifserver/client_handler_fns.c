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
	g_info("Attempting to read %lu bytes\n", *len);
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
			*len = 0;
			break;
		case SSL_ERROR_WANT_WRITE:
			g_warning("Read error on write, this doesn't make sense!\n");
			break;
		case SSL_ERROR_WANT_READ:
			g_warning("REad failed, retrying\n");
			goto retry_write;
			break;
		case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_WANT_ACCEPT:
			g_warning("Trying to work unconnected socket\n");
			delete_client(c->sd);
			*len = 0;
			break;
		case SSL_ERROR_SYSCALL:
			g_warning("syscall error: %s\n", strerror(errno));
			delete_client(c->sd);
			*len = 0;
			break;
		default:
			g_warning("Unhandled return code %d\n", rc);
			delete_client(c->sd);
			break;
	}

//out:
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

static int handle_get_server_info_req(struct client *c, GetServerInfoRequest *req)
{
	ServerResponse resp = SERVER_RESPONSE__INIT;
	GetServerInfoResponse gsir = GET_SERVER_INFO_RESPONSE__INIT;
	uint32_t len;
	uint8_t *buf = NULL;
	int rc = -ENOMEM;

	resp.submessage_case = SERVER_RESPONSE__SUBMESSAGE_GSIR;
	resp.gsir = &gsir;
	gsir.pem = "The Quick Brown Fox";
	len = server_response__get_packed_size(&resp);
	buf = calloc(len, sizeof(uint8_t));
	if (!buf) {
		g_warning("Unable to allocate buffer to send response\n");
		goto out;
	}
	server_response__pack(&resp, buf);
	rc = write_data_to_ssl(c, buf, len);
	free(buf);
out:	
	return rc;
}

static int handle_client_request(struct client *c){
	uint8_t buf[1024];
	size_t len = 1024;
	ClientRequest *msg;
	int rc = -ENOMEM;

	g_info("Got client request\n");
	rc = read_data_from_ssl(c, buf, &len);
	if (rc) {
		g_warning("Read failed\n");
		goto out;
	}
	g_info("Got %lu data bytes from ssl\n", len);
	msg = client_request__unpack(NULL, len, buf);
	if (msg == NULL) {
		g_warning("unknown message for client\n");
		delete_client(c->sd);
		goto out;
	}

	switch (msg->submessage_case) {
	case CLIENT_REQUEST__SUBMESSAGE_GSIR:
		rc = handle_get_server_info_req(c, msg->gsir);
		break;
	case CLIENT_REQUEST__SUBMESSAGE__NOT_SET:
	default:
		rc = -ENOENT;
		g_warning("Unknown message case %d\n", msg->submessage_case);
		delete_client(c->sd);
		goto out_free;
		break;
	}

out_free:
	client_request__free_unpacked(msg, NULL);
out:
	return rc;
}

int start_client_tls(struct client *c)
{
	g_info("Establishing SSL connection for new client\n");
	c->ssl =  SSL_new(c->ctx);
	SSL_set_fd(c->ssl, c->sd);
	if (SSL_accept(c->ssl) <= 0) {
		g_warning("Unable to establish ssh session\n");
		ERR_print_errors_fp(stderr);
		delete_client(c->sd);
		goto out;
	}

	c->handler = handle_client_request;
out:
	return 0;
}

