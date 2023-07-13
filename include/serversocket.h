#ifndef __SERVERSOCKET_H__
#define __SERVERSOCKET_H__

int setup_server_listening_socket(GMainLoop *loop, const char *keyfile, const char *certfile);
int shutdown_listening_socket();
#endif

