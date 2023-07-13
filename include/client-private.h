/**
 * \file client.h client connection handling
 */
#ifndef __CLIENT_PRIVATE_H__
#define __CLIENT_PRIVATE_H__
#include <sserver.h>
#include <openssl/opensslv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct client {
	int sd;
	GSource *source;
	int (*handler)(struct client *c);
	gboolean pending_exec;

	/* SSL data */
	SSL_CTX *ctx;
	SSL *ssl;	
};


int start_client_tls(struct client *c);

#endif

