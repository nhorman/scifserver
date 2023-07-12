/**
 * \file client.c client connection handling
 */
#include <sserver.h>
#include <glib-unix.h>
#include <client-private.h>

static GHashTable *clienttable = NULL;
static GThreadPool *workers = NULL;

static void process_client(gpointer data, gpointer user_data)
{
	int rc;
	struct client *c = data;

	rc = c->handler(c);
	if (rc) {
		g_error("handler function failed\n");
	}
	c->pending_exec = false;
	g_rc_box_release(c);
	return;
}

int setup_client_handling(int max_threads)
{
	int rc = -ENOMEM;

	clienttable = g_hash_table_new(g_int_hash, g_int_equal);
	if (!clienttable) {
		g_warning("Unable to create hash table\n");
		goto out;
	}

	workers = g_thread_pool_new(process_client, clienttable, max_threads, false, NULL);
	if (!workers) {
		g_warning("Unable to create thread pool\n");
		goto out;
	}

	rc = 0;
out:
	return rc;
}

int cleanup_client_handling()
{

	g_thread_pool_free(workers, true, true);
	workers = NULL;
	g_hash_table_destroy(clienttable);
	clienttable = NULL;
	return 0;
}


static gboolean client_socket_handler(gpointer arg)
{
	int sd = GPOINTER_TO_INT(arg); 
	GError *err = NULL;

	struct client *c = g_hash_table_lookup(clienttable, &sd);
	if (!c) {
		g_warning("Did not find client for sd %d\n", sd);
		goto out;
	}
	if (c->pending_exec) {
		goto out;
		return G_SOURCE_CONTINUE;
	}
	c->pending_exec = true;
	g_rc_box_acquire(c);
	if (!g_thread_pool_push(workers, c, &err)) {
		g_error("Failed to push new thread: %s\n", err->message);
	}

out:
	return G_SOURCE_CONTINUE;
}

int create_client(int sd, GMainLoop *loop, SSL_CTX *ctx)
{
	int rc = -ENOMEM;
	struct client *newc = g_rc_box_new(struct client);
	if (!newc) {
		goto out;
	}

	newc->sd = sd;
	newc->handler = start_client_tls;
	newc->source = g_unix_fd_source_new(sd, G_IO_IN);
	newc->pending_exec = false;
	newc->ctx = ctx;
	g_source_set_callback(newc->source, client_socket_handler, &newc->sd, NULL);
        g_source_attach(newc->source, g_main_loop_get_context(loop));	
	if (g_hash_table_insert(clienttable, newc, &newc->sd) != true) {
		g_error("Inserting sd %d twice!\n", sd);
	}

	/* Prime the state machien */
	g_rc_box_acquire(newc);
	process_client(newc, NULL);
	
	rc = 0;	
out:
	return rc;
}

int delete_client(int sd)
{
	struct client *c = g_hash_table_lookup(clienttable, &sd);
	g_hash_table_remove(clienttable, &sd);
	if (c->ssl) {
		SSL_shutdown(c->ssl);
		SSL_free(c->ssl);
	}
	close(c->sd);
	g_source_destroy(c->source);
	g_rc_box_release(c);
	return 0;
}
