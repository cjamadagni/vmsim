#ifndef STATS_H
#define STATS_H

#include <sys/types.h>
#include <stdio.h>

#include <vmsim.h>

typedef uint count_t;
typedef count_t type_count_t[REF_KIND_NUM];

typedef struct _stats {
  type_count_t references;
  type_count_t miss;
  type_count_t compulsory;
  type_count_t evictions;
  type_count_t evict_dirty;
  FILE *output;
} stats_t;

extern stats_t *stats;

void stats_init();
void stats_output();

static inline void stats_compulsory(ref_kind_t type) {
  stats->compulsory[type]++;
}

static inline void stats_reference(ref_kind_t type) {
  stats->references[type]++;
}

static inline void stats_miss(ref_kind_t type) {
  stats->miss[type]++;
}

static inline void stats_evict(ref_kind_t type) {
  stats->evictions[type]++;
}

static inline void stats_evict_dirty(ref_kind_t type) {
  stats->evict_dirty[type]++;
}


#endif /* STATS_H */
