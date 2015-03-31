#ifndef __TYPES_H_
#define __TYPES_H_

#include <stdint.h>
#include <l4/ipc.h>

/* FIXME: This should be moved to time.h */
struct timespec {
	uint64_t nsec;
};

struct posix_sem_t {
    uint16_t refcount;
    uint16_t value;
};
typedef struct posix_sem_t sem_t;

/* FIXME: Define proper type for pthread type */
typedef uint32_t pthread_mutex_t;
typedef uint32_t pthread_mutexattr_t;
typedef struct {
	L4_ThreadId_t ptid;
	int joinable;
	sem_t pthread_struct_sem;
} pthread_t;
typedef uint32_t pthread_attr_t;

#endif
