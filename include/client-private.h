/**
 * \file client.h client connection handling
 */
#ifndef __CLIENT_PRIVATE_H__
#define __CLIENT_PROVATE_H__
#include <sserver.h>

struct client {
	int sd;
	GSource *source;
	int (*handler)(struct client *c);
	gboolean pending_exec;
};

int create_client(int sd, GMainLoop *loop);

int start_client_tls(struct client *c);

#endif

