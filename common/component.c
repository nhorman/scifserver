#include <sserver.h>

CIRCLEQ_HEAD(components, component_info);

int start_components()
{
	int rc = 0;
	struct component_info *idx;
	CIRCLEQ_FOREACH(&components, idx, entries) {
		g_info("Starting component %s (prio %d)\n", idx->name, idx->prio);
		if (idx->start) {
			rc = idx->start();
			if (rc) {
				g_warning("Component %s failed startup with code %d\n", idx->name, rc);
				goto out;
			}
		}
	}
out:
	return rc;
}

void stop_components()
{
	int rc = 0;
	struct component_info *idx;
	CIRCLEQ_FOREACH_REVERSE(&components, idx, entries) {
		g_info("Stopping component %s (prio %d)\n", idx->name, idx->prio);
		if (idx->start) {
			rc = idx->stop();
			if (rc) {
				g_warning("Component %s failed stop with code %d\n", idx->name, rc);
			}
		}
	}
	return;
}


void register_component(struct component_info *info)
{
	struct component_info *idx;

	if (CIRCLEQ_EMPTY(&components)) {
		CIRCLEQ_INSERT_HEAD(&components, info, entries);
		goto out;
	}

	CIRCLEQ_FOREACH(idx, &components, entries) {
		if (info->prio <= idx->prio) {
			CIRCLEQ_INSERT_BEFORE(&components, idx, info, entries);
			goto out;
		}
		if (idx == CIRCLEQ_LAST(&components)) {
			/* last entry, just append */
			CIRCLEQ_INSERT_AFTER(&components, idx, info, entries);
			goto out;
		}
	}
out:
	return;
}


static void __attribute__((constructor(100))) init_component_list()
{
	CIRCLEQ_INIT(&components);
}
