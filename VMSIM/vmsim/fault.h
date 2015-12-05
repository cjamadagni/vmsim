/*
 * fault.h - Declares the available fault handlers.
 *
 */
#ifndef FAULT_H
#define FAULT_H

#include <unistd.h>
#include <vmsim.h>
#include <pagetable.h>

/* fault handlers are functions that return nothing (void) and
 * take 2 arguments: a pte_t* and a ref_kind_t.
 * The pte is the new page that must be inserted, and the
 * type is for statistical reporting. */
typedef void (*fault_handler_t)(pte_t *pte, ref_kind_t type);

/* fault_handler_info_t lets us match the actual function to a
 * name. fault_handlers is searched by options.c to locate the
 * handler named on the command line. */
typedef struct _fault_handler_info {
  char *name;
  fault_handler_t handler;
} fault_handler_info_t;

extern fault_handler_info_t fault_handlers[];

/* Initialize any state needed by this module. */
void fault_init();

#endif /* FAULT_H */
