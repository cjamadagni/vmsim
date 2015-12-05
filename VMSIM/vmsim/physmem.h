/*
 * physmem.h - The physmem module models the physical memory pages.
 *
 */

#ifndef PHYSMEM_H
#define PHYSMEM_H

#include <vmsim.h>
#include <pagetable.h>

/* Initialize physical memory to all-empty. */
void physmem_init();

/* Get an array of pte_ts representing physical memory.
 * Do not modify this array directly; do not modify elements of it directly.
 * Use physmem_evict/physmem_load.
 * There are opts.phys_pages elements in the array.
 * Empty elements are NULL. */
pte_t** physmem_array();

/* Evict the page at the given pfn from memory. type should specify
 * the type of reference casuing the eviction (i.e., the type passed
 * to the fault handler). Will mark the pfn as empty (suitable for
 * physmem_load). */
void physmem_evict(uint pfn, ref_kind_t type);

/* Load the given page (pte) into the given physical memory slot (pfn).
 * That slot should be empty (either because it has never been used, or
 * because the page there has been evicted). type should specify what
 * kind of reference casused the load. */
void physmem_load(uint pfn, pte_t *pte, ref_kind_t type);
void physmem_dump();
extern pte_t **physmem;

#endif /* PHYSMEM_H */
