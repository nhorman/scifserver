/** @file
 *
 * \brief software component infrastucture functions
 *
 * This file implements the api to do common componenttem registration functions
 *
 *
 */

#include <sserver.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "component.h"

static struct component_register_info *component_list = NULL;
static struct component_register_info *component_end = NULL;

static pthread_mutex_t component_lock = PTHREAD_MUTEX_INITIALIZER;
static bool component_immutable = false;

void register_component(struct component_register_info *ops)
{
	assert(component_immutable == false);

	if ((ops->priority >= COMPONENT_PRIO_MAX) || (ops->priority < COMPONENT_PRIO_0)) {
		fprintf(stderr, "Build bug!  Invalid priority for component %s\n", ops->name);
		exit(1);
	}

	/*
	 * We append to the end of the component list here because we use the
	 * constructor priority for registration order, giving us a priority
	 * ordered list
	 */
	if (!component_list) {
		component_list = ops;
		component_end = component_list;
	} else {
		component_end->next = ops;
		component_end = ops;
	}
	ops->next = NULL;
}

static void lock_and_check_components()
{
	struct component_register_info *idx;
	component_prio_t last_prio = 0;

	pthread_mutex_lock(&component_lock);
	if (component_immutable) {
		goto out;
	}

	/* prevent further component registrations */
	component_immutable = true;

	for (idx = component_list; idx != NULL; idx = idx->next) {
		if ((idx->priority < COMPONENT_EARLY_INIT) || (idx->priority > COMPONENT_LATE_INIT)) {
			if (idx->priority == last_prio) {
				fprintf(stderr, "Priority %d can have only one registrant, %s fails\n", idx->priority, idx->name);
				abort();
			}
			last_prio = idx->priority;
		}
	}

out:
	pthread_mutex_unlock(&component_lock);
	return;
}

int config_components()
{
	struct component_register_info *idx;
	int rc = 0;

	lock_and_check_components();

	pthread_mutex_lock(&component_lock);
	for (idx = component_list; idx != NULL; idx = idx->next) {
		fprintf(stderr, "Configuring %s componenttem (prio %d)\n", idx->name, idx->priority);
		if (idx->cfg) {
			rc = idx->cfg();
			if (rc) {
				fprintf(stderr, "Init of componenttem %s failed\n", idx->name);
				goto out;
			}
		}
	}

out:
	pthread_mutex_unlock(&component_lock);
	return rc;
}

int init_components()
{
	struct component_register_info *idx;
	int rc = 0;

	lock_and_check_components();

	pthread_mutex_lock(&component_lock);
	for (idx = component_list; idx != NULL; idx = idx->next) {
		fprintf(stderr, "Initalizing %s componenttem (prio %d)\n", idx->name, idx->priority);
		if (idx->init) {
			rc = idx->init();
			if (rc) {
				fprintf(stderr, "Init of componenttem %s failed\n", idx->name);
				goto out;
			}
		}
	}

out:
	pthread_mutex_unlock(&component_lock);
	return rc;
}

int start_components()
{
	struct component_register_info *idx;
	int rc = 0;

	lock_and_check_components();

	pthread_mutex_lock(&component_lock);
	for (idx = component_list; idx != NULL; idx = idx->next) {
		fprintf(stderr, "Starting %s componenttem (prio %d)\n", idx->name, idx->priority);
		if (idx->start) {
			rc = idx->start();
			if (rc) {
				goto out;
			}
		}
	}

out:
	pthread_mutex_unlock(&component_lock);
	return rc;
}


static void reverse_list()
{
	struct component_register_info *prevNode, *curNode;

	if (component_list != NULL) {
		prevNode = component_list;
		curNode = component_list->next;
		component_list = component_list->next;

		prevNode->next = NULL; // Mark first node as last node

		while (component_list != NULL) {
			component_list = component_list->next;
			curNode->next = prevNode;

			prevNode = curNode;
			curNode = component_list;
		}

		component_list = prevNode; // Make last node as head
	}
}


void shutdown_components()
{
	struct component_register_info *idx;

	pthread_mutex_lock(&component_lock);
	reverse_list();
	for (idx = component_list; idx != NULL; idx = idx->next) {
		fprintf(stderr, "Stopping %s componenttem (prio %d)\n", idx->name, idx->priority);
		if (idx->shutdown) {
			idx->shutdown();
		}
	}

	pthread_mutex_unlock(&component_lock);
}

