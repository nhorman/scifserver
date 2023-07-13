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

SSL_CTX *ctx = NULL;

int main(int argc, char **argv)
{
	int sd;
	struct sockaddr_in addr;
	int rc = -EFAULT;
	SSL *ssl = NULL;
	const SSL_METHOD *method = NULL;
	
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
		close(sd);
		ERR_print_errors_fp(stderr);
		g_warning("Unable to complete ssl handshake\n");
	}

	SSL_free(ssl);
	close(sd);	
	SSL_CTX_free(ctx);
	OPENSSL_cleanup();
	rc = 0;
	 
	return rc;
}

