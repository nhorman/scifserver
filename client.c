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
	g_rc_box_release(c);
	c->pending_exec  = false;
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

void shutdown_client_handling()
{

	g_thread_pool_free(workers, false, true);
	workers = NULL;
	g_hash_table_destroy(clienttable);
	clienttable = NULL;
}


static gboolean client_socket_handler(gpointer arg)
{
	int sd = *((int *)arg);
	GError *err = NULL;

	struct client *c = g_hash_table_lookup(clienttable, &sd);
	if (!c) {
		g_error("Did not find client for sd %d\n", sd);
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

int create_client(int sd, GMainLoop *loop)
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
	g_source_set_callback(newc->source, client_socket_handler, &newc->sd, NULL);
        g_source_attach(newc->source, g_main_loop_get_context(loop));	
	if (g_hash_table_insert(clienttable, newc, &newc->sd) != true) {
		g_error("Inserting sd %d twice!\n", sd);
	}
	
	rc = 0;	
out:
	return rc;
}


