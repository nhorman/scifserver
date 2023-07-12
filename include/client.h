/**
 * \file client.h client connection handling
 */
#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <sserver.h>

int setup_client_handling(int max_threads);
void shutdown_client_handling();

int create_client(int sd, GMainLoop *loop);

#endif

