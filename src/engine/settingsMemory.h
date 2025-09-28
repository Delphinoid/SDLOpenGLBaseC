#ifndef MEMORYSETTINGS_H
#define MEMORYSETTINGS_H

///#define MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE 536870912
///#define MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP

// This is probably a good idea on Linux, as SDL's
// calls to malloc appear to screw with sbrk.
///#define MEMORY_ALLOCATOR_USE_MALLOC

#define MEMORY_ALLOCATOR_ALIGNED
#define MEMORY_DEBUG

#endif
