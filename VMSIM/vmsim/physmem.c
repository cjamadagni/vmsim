//#include <config.h>
#include <assert.h>
#include <stdlib.h>

#include <options.h>
#include <pagetable.h>
#include <physmem.h>
#include <stats.h>

pte_t **physmem;

void physmem_init() {
  physmem = (pte_t**)(calloc(opts.phys_pages, sizeof(pte_t*)));
  assert(physmem);
}

pte_t **physmem_array() {
  return physmem;
}

void physmem_evict(uint pfn, ref_kind_t type) {
  assert(0 <= pfn && pfn < opts.phys_pages);

  /* No page here - nothing to do */
  if(physmem[pfn] == NULL || !physmem[pfn]->valid) {
    physmem[pfn] = NULL;
    return;
  }

#ifdef DEBUG
  printf("Evicting page frame with pfn=0x%x to disk\n", pfn);
  //printf("Evicting page frame with pfn=0x%x, type=%c to disk\n", pfn, type==REF_KIND_LOAD? 'R':'W');
#endif
  stats_evict(type);
  if (physmem[pfn]->modified) {
    stats_evict_dirty(type);
  }
  physmem[pfn]->frequency=0;
  physmem[pfn]->modified = 0;
  physmem[pfn]->valid = 0;
  physmem[pfn] = NULL;
}

void physmem_load(uint pfn, pte_t *new_page, ref_kind_t type) {
  assert(0 <= pfn && pfn < opts.phys_pages);
  assert(new_page && !new_page->valid);
  assert(physmem[pfn] == NULL);

  physmem[pfn] = new_page;

  physmem[pfn]->pfn = pfn;
  physmem[pfn]->reference = 0;
  physmem[pfn]->modified = 0;
  physmem[pfn]->valid = 1;
}

void physmem_dump() {
  uint i;
 // printf("physmem fields pfn:valid:reference:modified\n");
  printf("\nCurrent physmem pte fields.\tvalid  \tvfn  \tpfn         modified        reference        counter       ResetCounter          frequency\n");

  for(i = 0 ; i < opts.phys_pages; i++) {
                 pte_t *pte=(pte_t *) physmem[i];
		 if (pte) {
                 printf("physmem[0x%x]: \t\t\t%d  \t0x%x  \t0x%x  \t\t%d  \t\t%d \t\t%d \t\t %d \t\t      %d\n",
                          i,
                          pte->valid,
                          pte->vfn,
                          pte->pfn,
                          pte->modified,
                          pte->reference,
			  pte->counter,
                          pte->c,
			  pte->frequency);
		}
  }
}
