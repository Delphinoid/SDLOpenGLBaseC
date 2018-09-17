#include "memoryShared.h"

#define memRegionAppendMacro(first, region) \
	while(*first != NULL){ \
		first = (memoryRegion **)(&(*first)->next); \
	} \
	region->next = NULL; \
	*first = region;

#define memRegionPrependMacro(first, region) \
	region->next = *first; \
	*first = region;

inline void memRegionAppend(memoryRegion **first, memoryRegion *region, byte_t *data){
	/*
	** Appends a new memory region
	** to the list. Used when order
	** does matter.
	*/
	#if !defined(MEMORY_FORCE_ORDERLESS_EXTENSIONS) || defined(MEMORY_FORCE_ORDERED_EXTENSIONS)
		memRegionAppendMacro(first, region);
	#else
		memRegionPrependMacro(first, region);
	#endif
	region->start = data;
}

inline void memRegionPrepend(memoryRegion **first, memoryRegion *region, byte_t *data){
	/*
	** Prepends a new memory region
	** to the list. Used when order
	** doesn't matter.
	*/
	#if !defined(MEMORY_FORCE_ORDERED_EXTENSIONS) || defined(MEMORY_FORCE_ORDERLESS_EXTENSIONS)
		memRegionPrependMacro(first, region);
	#else
		memRegionAppendMacro(first, region);
	#endif
	region->start = data;
}

inline void memRegionFree(memoryRegion *region){
	/*
	** Frees an allocator by looping through
	** each of its memory regions.
	*/
	while(region != NULL){
		memoryRegion *next = (memoryRegion *)region->next;
		free(region->start);
		region = next;
	}
}
