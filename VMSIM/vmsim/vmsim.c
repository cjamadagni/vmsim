//#include <config.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <vmsim.h>
#include <util.h>
#include <options.h>
#include <pagetable.h>
#include <physmem.h>
#include <stats.h>
#include <fault.h>

void init();
void test();
void simulate();

/* refs per '.' printed */
uint dot_interval = 100;
uint dots_per_line = 64;

int ref_counter = 0;
int r=0;

int main(int argc, char **argv) {
	options_process(argc, argv);
	if (opts.test) {
    		test();
   	 	printf("Tests done.\n");
    		exit(0);
  	}
  
  	init();
  	simulate();
  	stats_output();
  
	return 0;
}

void init() {   
  pagetable_init();
  physmem_init();
  stats_init();
}

void test() {
  printf("Running vmtrace tests...\n");
  util_test();
  stats_init();
  pagetable_test();
}

ref_kind_t get_type(char c)
{
	if (c == 'R') return REF_KIND_LOAD;
	if (c == 'W') return REF_KIND_STORE;
	return REF_KIND_CODE;
}


void simulate() {
  uint pid;
  char ch;
  vaddr_t vaddr;
  ref_kind_t type;
  pte_t *pte;
  fault_handler_t handler;
  uint count = 0;
  FILE *fin = NULL;	
#ifdef DEBUG
  char response[20];
  uint pgfault=FALSE;
#endif
  
  handler = opts.fault_handler->handler;
  
  if ((fin=fopen(opts.input_file, "r")) == NULL) {
	  fprintf(stderr, "\n Could not open input file %s.", opts.input_file);
	exit(1);
  }
   printf("\n\nStarting simulation: ");
  printf("vaddr (Virtual Address) has %d bits, consisting of higher %d bits for vfn (Virtual Frame Number), and lower %d bits for offset within each page (log_2(pagesize=%d))\n",
	addr_space_bits, vfn_bits, log_2(opts.pagesize), opts.pagesize);
  while (fscanf(fin, "%d, %c, %x", &pid, &ch, &vaddr) !=EOF) {
	  type = get_type(ch);
	  stats_reference(type);
	  count++;
    
	  if (opts.verbose && (count % dot_interval) == 0) {
		  printf(".");
		  fflush(stdout); 
		  if ((count % (dots_per_line * dot_interval)) == 0) { 
			  printf("\n"); 
			  fflush(stdout); 
		  }
	  }
    
    pte = pagetable_lookup_vaddr(vaddr_to_vfn(vaddr), type);
#ifdef DEBUG
    //printf("Got the count=%dth memory ref with pid:%d mode:%c vaddr:0x%x vfn:0x%x(=top %d bits of %d-bit vaddr)\n",
//	count, pid, ch, vaddr, vaddr_to_vfn(vaddr), vfn_bits, addr_space_bits);
    printf("\nGot the count=%dth memory ref with pid:%d mode:%c vaddr:0x%x vfn:0x%x\n",
	count, pid, ch, vaddr, vaddr_to_vfn(vaddr));
      pgfault=!pte->valid;
      printf("\nGot a page %s. Do you want to dump out the page table and physmem? y or n: ", pgfault? "fault":"hit");
      scanf("%s", response);
#endif
    if (!pte->valid) { /* Fault */
      stats_miss(type);
      handler(pte, type);
	pte->c=r++;
    } 

    if(pte->valid) //for LFU and MFU , "chance" being modified for the Second chance algorithm
    {	
	pte->frequency = pte->frequency + 1;	
	pte->used = 1;
	pte->chance = 1;
    }
    
    pte->reference = 1;
    pte->counter = ref_counter++; //used by LRU

    if (type == REF_KIND_STORE)
      pte->modified = TRUE;

#ifdef DEBUG
   //printf("Page %s", pgfault? "Fault!\n": "Hit!\n");
      if (response[0]=='Y' || response[0]=='y') {
	pagetable_dump();
	physmem_dump();
	response[0]='N';
      }
#endif
    if (opts.limit && count >= opts.limit) {
      if (opts.verbose)
	printf("\nvmsim: reached %d references\n", count);
      break;
    }

  }
}

