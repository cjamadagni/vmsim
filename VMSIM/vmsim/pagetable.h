#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <vmsim.h>

//Default values that can be overwritten from the command line
const static int pagesize = 4096;
const static int log_pagesize = 12;

typedef struct _pte {
  uint           vfn; /* Virtual frame number */
  uint           pfn; /* Physical frame number iff valid=1 */
  int           reference;
  bool_t        valid; /* True if in physmem, false otherwise */
  bool_t        modified;
  int 		counter;  /* used for LRU, FIFO */
  int		frequency; /*used for LFU and MFU */
  int 		c; //keeping track of FIFO order in LFU and MFU
  int		used; //the used bit for clock algorithm
  int           chance; //The modification bit for second chance algorithm	

} pte_t;

/*
void pte_print(pte_t *pte)
  assert(pte);
             pte_t *pte=(pte_t *) root_table->table[i];
             printf("%d:0x%x:0x%x:%d:%d:%d\n",  
                          pte->valid,
                          pte->pfn,
                          pte->modified,
                          pte->reference,
                          pte->counter);
          }   
  }
*/
void pagetable_init();

/* Lookup the page table entry for the given virtual page.
 * If the page is not in memory, will have valid==0.
 * If the vfn has never been seen before, will create a new pte_t
 * with the given vfn and valid==0.
 * type is for statistical tracking.
 */
pte_t *pagetable_lookup_vaddr(uint vfn, ref_kind_t type);

void pagetable_test();

void pagetable_dump();
#endif /* PAGETABLE_H */
