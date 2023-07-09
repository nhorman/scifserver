/** @file
 *
 * \brief software platform infrastucture functions
 *
 * This file defines the api to do common platform registration functions
 *
 *
 */

#ifndef __COMPONENT_REG_H__
#define __COMPONENT_REG_H__
#include <sserver.h>
#include <stdbool.h>

#define __componentreg(p) __attribute__((constructor(p)))


/**
 * @brief registration priority.
 * The first 3 priorities are absolute, and only one componenttem can register
 * at that level, the remaining can have an arbitrary number register
 * Note: prioriy values begin at 101, as prio 0-100 is compiler reserved
 *
 * *COMPONENT_PRIO_[0-2] are the highest priority registration levels, and only a
 * single componenttem may register for each slot.  Multiple registrations will
 * result in runtime errors
 *
 * COMPONENT_EARLY_INIT -  priority for componenttems that should be registered before
 * any core systems come online (i.e. log componenttem)
 *
 * COMPONENT_DRIVER - priority for any hardware support functionality that might be
 * needed by higher level systems
 *
 * COMPONENT_CORE - core system functionality, netlib/crypto, etc
 *
 * COMPONENT_LATE_INIT - high level management functions that require all other
 * componenttems to be online prior to starting
 *
 * COMPONENT_PRIO_L[0-2] - like the fixed high priority systems, but lowset
 * priority, guaranteed to execute last and in order.
 */
typedef enum {
	COMPONENT_PRIO_DEPRECATIONS	= 101,
	COMPONENT_PRIO_0			= 102,
	COMPONENT_PRIO_1,
	COMPONENT_PRIO_2,
	COMPONENT_EARLY_INIT,
	COMPONENT_NAMESPACE,
	COMPONENT_DRIVER,
	COMPONENT_CORE,
	COMPONENT_LATE_INIT,
	COMPONENT_PRIO_L0,
	COMPONENT_PRIO_L1,
	COMPONENT_PRIO_L2,
	COMPONENT_PRIO_MAX
} component_prio_t;

/**
 * @brief A structure to define a componenttem in microedge
 */
struct component_register_info {
	char *				name;           /**< The name of the componenttem */
	int				(*cfg)();       /**< Called to parse config section for the platform */
	int				(*init)();      /**< Called to init the platform */
	int				(*start)();     /**< Called to start the subystem after init is complete */
	void				(*shutdown)();  /**< Called to shutdown the componenttem */
	component_prio_t			priority;       /**< The priority at which to register this componenttem */
	struct component_register_info *	next;           /**< Internal tracking pointer */
};

/*! \fn void register_component()
 * \brief register a component based on the component_register_info struct
 *  This should always and only be called from the REGISTER_COMPONENT macro
 * \param ops - log_register_info struct to define the logger backend
 */
void register_component(struct component_register_info *ops);

/*! \def ARRAY_SIZE(arr)
 * \brief macro to compute the number of elements in an array at compile time
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*! \def REGISTER_COMPONENT(name, init, start, shutdown)
 * \brief macro to register componenttems
 * instancing this macro in a file will automaticaly register the defined
 * componenttem at startup via a constructor
 * \param name - the name of the componenttem
 * \param config - the componenttem config parsing function
 * \param init - the componenttem init function
 * \param cfn - the componenttem start function
 * \param efn - the componenttem shutdown function
 * \param prio - the priority of the registration
 */
#define REGISTER_COMPONENT(name, config, init, start, shutdown, prio) \
	static struct component_register_info __ ## name ## _info = { \
		#name, \
		config, \
		init, \
		start, \
		shutdown, \
		prio, \
		NULL \
	}; \
	void __componentreg(prio) register_ ## name ## _component() { \
		register_component(&__ ## name ## _info); \
	} \


/*! \fn config_componentts()
 * \brief configure all components
 * \returns 0 on success
 * \returns < 0 on failure
 */
int config_components();

/*! \fn int init_components()
 * \brief initalize all components
 * \returns 0 on success
 * \returns < 0 on failure
 */
int init_componentts();


/*! \fn int start_componentts()
 * \brief start all components
 * \returns 0 on success
 * \returns < 0 on failure
 */
int start_componentts();

/*! \fn int shutdown_components()
 * \brief shutdown all components
 */
void shutdown_components();

#endif
