#ifndef VMSIM_H
#define VMSIM_H

#include <sys/types.h>

#define FALSE 0
#define TRUE 1

typedef int bool_t;
typedef unsigned char byte_t;

typedef uint vaddr_t;
typedef uint paddr_t;

typedef enum _ref_kind {
  REF_KIND_CODE=0, REF_KIND_LOAD=1, REF_KIND_STORE=2
} ref_kind_t;

#define REF_KIND_NUM 3

const static uint addr_space_bits = 16;
extern uint vfn_bits;
extern int ref_counter;

#endif /* VMSIM_H */
