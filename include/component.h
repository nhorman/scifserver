/**
 * \file component.h component abstraction
 */
#ifndef __COMPONENT_H__
#define __COMPONENT_H__
#include <sys/queue.h>

int start_components();

void stop_components();


struct component_info {
	char *name;
	int prio;
	int (*start)();
	int (*stop)();
	CIRCLEQ_ENTRY(component_info) entries;
};

void register_component(struct component_info *info);

#define COMPONENT_PRIO_EARLY 102
#define COMPONENT_PRIO_CORE 103
#define COMPONENT_PRIO_LATE 104

#define __componentreg(prio) __attribute__((constructor(prio)))

#define REGISTER_COMPONENT(name, start, stop, prio) \
        static struct component_info __ ## name ## _info = { \
                #name, \
                prio, \
                start, \
                stop, \
        }; \
        void __componentreg(prio) register_ ## name ## _component() { \
                register_component(&__ ## name ## _info); \
        } \

#endif

