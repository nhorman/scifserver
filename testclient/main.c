#include <sserver.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <messages.pb-c.h>

SSL_CTX *ctx = NULL;

static int read_data_from_ssl(SSL *ssl, void *buf, size_t *len)
{
        int rc;

retry_write:
        memset(buf, 0, *len);
        g_info("Attempting to read %lu bytes\n", *len);
        rc = SSL_get_error(ssl, SSL_read_ex(ssl, buf, *len, len));
        switch (rc) {
                case SSL_ERROR_NONE:
                        break;
                case SSL_ERROR_SSL:
                        g_warning("Unrecoverable ssl error\n");
                        *len = 0;
                        break;
                case SSL_ERROR_ZERO_RETURN:
                        g_info("Peer has closed connection, deleting client\n");
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
                        *len = 0;
                        break;
                case SSL_ERROR_SYSCALL:
                        g_warning("syscall error: %s\n", strerror(errno));
                        *len = 0;
                        break;
                default:
                        g_warning("Unhandled return code %d\n", rc);
                        break;
        }

        return rc;
}


static int write_data_to_ssl(SSL *ssl, void *buf, uint32_t len)
{
        int rc;
retry_write:
        rc = SSL_get_error(ssl, SSL_write(ssl, buf, len));
        switch (rc) {
                case SSL_ERROR_NONE:
                        break;
                case SSL_ERROR_SSL:
                        g_warning("Unrecoverable ssl error\n");
                        break;
                case SSL_ERROR_ZERO_RETURN:
                        g_info("Peer has closed connection, deleting client\n");
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
                        break;
                case SSL_ERROR_SYSCALL:
                        if (errno == -EPIPE) {
                                g_warning("Remote client closed connection\n");
                        } else {
                                g_warning("syscall error: %s\n", strerror(errno));
                        }
                        break;
                default:
                        g_warning("Unhandled return code %d\n", rc);
                        break;
        }

        return rc;
}

static void print_gsir_response(GetServerInfoResponse *gsir)
{
	g_info("PEM string for server is %s\n", gsir->pem);
}

static int read_server_response(SSL *ssl)
{
	size_t len = 1024;
	uint8_t buf[len];
	int rc;
	ServerResponse *resp;

	rc = read_data_from_ssl(ssl, buf, &len);
	if (rc) {
		goto out;
	}

	resp = server_response__unpack(NULL, len, buf);
	if (!resp) {
		rc = EFAULT;
		g_warning("Failed to decode message\n");
		goto out;
	}

	switch(resp->submessage_case) {
	case SERVER_RESPONSE__SUBMESSAGE_GSIR:
		print_gsir_response(resp->gsir);
		break;
	default:
		g_warning("unknown submessage type %d\n", resp->submessage_case);
		rc = -EINVAL;
		break;
	}

	server_response__free_unpacked(resp, NULL);	
out:
	return rc;
}

static int send_server_info_request(SSL *ssl)
{
	ClientRequest req = CLIENT_REQUEST__INIT;
	GetServerInfoRequest gsir = GET_SERVER_INFO_REQUEST__INIT;
	uint32_t len;
	uint8_t *buf;
	int rc = -ENOMEM;

	req.submessage_case = CLIENT_REQUEST__SUBMESSAGE_GSIR;
	req.gsir = &gsir;
	gsir.magic = "This is a get server information request messge";
	len = client_request__get_packed_size(&req);
	g_info("Client request is %d bytes\n", len);
	buf = calloc(len, sizeof(uint8_t));
	if (!buf) {
		g_warning("Unable to allocate buffer to send info request\n");
		goto out;
	}
	client_request__pack(&req, buf);
	rc = write_data_to_ssl(ssl, buf, len);
        free(buf);
out:
	return rc;
}

static void log_handler(const gchar *domain, GLogLevelFlags log_level, const gchar *message, gpointer data)
{
	fprintf(stderr, "%s: %s", domain ? domain : "testclient", message);
}

int main(int argc, char **argv)
{
	int sd;
	struct sockaddr_in addr;
	int rc = -EFAULT;
	SSL *ssl = NULL;
	const SSL_METHOD *method = NULL;

	g_log_set_default_handler(log_handler, NULL);

	SSL_library_init();
	OPENSSL_add_all_algorithms_noconf();
        ERR_load_crypto_strings();
        SSL_load_error_strings();

	method = TLS_client_method();
	ctx = SSL_CTX_new(method);
	if (!ctx) {
		g_error("Unable to create ssl context\n");
	}

	sd = socket(PF_INET, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4443);
	inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));
	if (connect(sd, (const struct sockaddr *)&addr, sizeof(addr)) != 0) {
		rc = errno;
		close(sd);
		g_error("Unable to connect to server: %s\n", strerror(rc));
	}
	ssl = SSL_new(ctx);	
	if (SSL_set_tlsext_host_name(ssl, "localhost") == 0) {
		rc = errno;
		close(sd);
		g_error("Unable to set hostname\n");
	}
	SSL_set_fd(ssl, sd);
	if (SSL_connect(ssl) == -1) {
		ERR_print_errors_fp(stderr);
		g_warning("Unable to complete ssl handshake\n");
		goto out_free;
	}

	rc = send_server_info_request(ssl);
	if (rc) {
		g_warning("Unable to send server info request\n");
		goto out_free;
	}

	rc = read_server_response(ssl);
	if (rc) {
		g_warning("Unable to read server info response\n");
		goto out_free;
	}

	rc = 0;
out_free:
	SSL_free(ssl);
	close(sd);	
	SSL_CTX_free(ctx);
	OPENSSL_cleanup();
	 
	return rc;
}

