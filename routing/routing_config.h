

#ifndef __ROUTING_CONFIG_H
#define __ROUTING_CONFIG_H

#ifdef FORCE_HOPS
	#undef FORCE_HOPS
	#define FORCE_HOPS	1
#else
	#define FORCE_HOPS 	0
#endif

#endif // __ROUTING_CONFIG_H