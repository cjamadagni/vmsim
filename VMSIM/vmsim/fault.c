/*
 * fault.c - Defines the available fault handlers. One example,
 *           fault_random, is provided; be sure to add your handlers 
 	     to fault_handlers[].
 *
 */

#include <vmsim.h>
#include <fault.h>
#include <options.h>
#include <physmem.h>
#include <stdlib.h>
#include <stdio.h>

static void fault_random(pte_t *pte, ref_kind_t type);
static void fault_lfu(pte_t *pte, ref_kind_t type);
static void fault_mfu(pte_t *pte, ref_kind_t type);
static void fault_lru(pte_t *pte, ref_kind_t type);
static void fault_fifo(pte_t *pte, ref_kind_t type);
static void fault_clock(pte_t *pte, ref_kind_t type);
static void fault_second(pte_t *pte, ref_kind_t type);
static void fault_init_random();

fault_handler_info_t fault_handlers[8] = {
  { "random", fault_random },
  { "lfu", fault_lfu },
  { "lru", fault_lru },
  { "fifo", fault_fifo },
  { "mfu", fault_mfu },
  { "clock", fault_clock },
  { "second", fault_second }, 
  { NULL, NULL } /* last entry must always be NULL/NULL */
};

/* Initialize any state needed by the fault handlers here.
 * For example, the random number generator must be initialized for
 * fault_random. */
void fault_init() {
  fault_init_random();
}

void fault_init_random() {
  long seed = 1234567;
  srandom(seed);
}


//Random page replacement algorithm
void fault_random(pte_t *pte, ref_kind_t type) {
  int page;
  page = random() % opts.phys_pages;
  physmem_evict(page, type);
  physmem_load(page, pte, type);
}


//least frequetly used algorithm - LFU
void fault_lfu(pte_t *pte, ref_kind_t type) {

	//printf(" \n\n lfu not implemented yet \n\n");

	static int frame = 0;
	
	int i;

	pte_t *test=(pte_t*)malloc(sizeof(pte_t));

	//int min_frequency = 0;
	int loc = 0;

	static int check = 1;
	
	//for FAULT
	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		
		frame = frame + 1;
		check = check + 1;			

	}

	else
	{

		//min_frequency = physmem[0]->frequency;
		test->frequency = physmem[0]->frequency;
		for(i=0;i<opts.phys_pages;i++)
		{

			if(test->frequency > physmem[i]->frequency)
			{
				test->frequency = physmem[i]->frequency;
				loc = i;
				//min_counter = physmem[i]->frequency;
				test->c = physmem[i]->c;
			}

			
		}

		//to maintain FIFO order
		for(i=0;i<opts.phys_pages;i++)
		{
			if((physmem[i]->frequency == test->frequency) && (physmem[i]->c < test->c))
				loc = i;
		}

		physmem_evict(loc, type);
  		physmem_load(loc, pte, type);
		
	
	}	


}



//Most frequetly used algorithm - MFU
void fault_mfu(pte_t *pte, ref_kind_t type) {

	//printf(" \n\n mfu not implemented yet \n\n");

	static int frame = 0;
	
	int i;

	pte_t *test=(pte_t*)malloc(sizeof(pte_t));

	
	int loc = 0;

	static int check = 1;
	
	//for FAULT
	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		
		frame = frame + 1;
		check = check + 1;			

	}

	else
	{

		
		test->frequency = physmem[0]->frequency;
		for(i=0;i<opts.phys_pages;i++)
		{

			if(test->frequency < physmem[i]->frequency)
			{
				test->frequency = physmem[i]->frequency;
				loc = i;
				//min_counter = physmem[i]->frequency;
				test->c = physmem[i]->c;
			}

			
		}

		
		for(i=0;i<opts.phys_pages;i++)
		{
			if((physmem[i]->frequency == test->frequency) && (physmem[i]->c < test->c))
				loc = i;
		}

		physmem_evict(loc, type);
  		physmem_load(loc, pte, type);
		
	
	}	

}



// LRU replacement 
void fault_lru(pte_t *pte, ref_kind_t type) {
	//printf("LRU not implemented yet!\n");

	static int frame = 0;

	int i,location=0;

	

	int minimum=0;


	static int check = 1;


	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		frame = frame + 1;
		check = check + 1;			

	}

	else
	{
		
		minimum = physmem[0]->counter;
		for(i=0;i<opts.phys_pages;i++)
		{
			if(minimum > physmem[i]->counter)
			{
				minimum = physmem[i]->counter;
				location = i;
			} 
		}

		physmem_evict(location, type);		
		physmem_load(location, pte, type);

	

	}


}


// FIFO replacement 
void fault_fifo(pte_t *pte, ref_kind_t type) {
	//printf("FIFO not implemented yet!\n");

	
	int i,loc=0,min;
	static int frame = 0;
	static int check = 1;

	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		frame = frame + 1;
		check = check + 1;
					

	}

	else
	{

	min = physmem[0]->c;
	for(i = 0;i<opts.phys_pages;i++)
	{

		if(physmem[i]->c < min)
		{
			min = physmem[i]->c;
			loc = i;	
		}

	}

	physmem_evict(loc, type);		
	physmem_load(loc, pte, type);

	

}
}

//Clock replacement algorithm
static void fault_clock(pte_t *pte, ref_kind_t type) {

	static int check = 1;
	static int frame = 0;
	int i,loc=0;
	static int clock_hand = 0;

	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		physmem[frame]->used = 1;
		frame = frame + 1;
		check = check + 1;
					

	}

	else
	{
		for(i=clock_hand;i<opts.phys_pages;i++)
		{
			if(physmem[i]->used == 0)
			{
				loc = i;
				break;
			}
			
			else
			{
				physmem[i]->used = 0;
			}

			if((i+1) == opts.phys_pages )
				i = -1;	


		}

				
		physmem_evict(loc, type);  
		physmem_load(loc, pte, type);

		clock_hand++;

		if(clock_hand == opts.phys_pages)
			clock_hand = 0;
		

		
		
	}


}


//Second chance algorithm - A variant of FIFO
static void fault_second(pte_t *pte, ref_kind_t type)
{


	//printf("\n\nSecond clock algorithm not implemented yet\n\n");
	static int check = 1;
	static int frame = 0;
	int i,j;
	int loc = 0;
	//int loc2=0,min=0;

	//pte_t *temp =(pte_t *)malloc(sizeof(pte_t));
	pte_t *temp;
	//Keep loading till all frames in memory are filled
	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		frame = frame + 1;
		check = check + 1;
					

	}

	//Finding the minimum
	

	else
	{
		for(i=0;i<opts.phys_pages;i++)
		{
			for(j=0;j<(opts.phys_pages-i-1);j++)
			{
				if(physmem[j+1]->c < physmem[j]->c)
				{
					temp = physmem[j];
					physmem[j] = physmem[j+1];
					physmem[j+1] = temp;
				}
			}
		}

		
		for(i=0;i<opts.phys_pages;i++)
		{
			if(physmem[i]->chance == 1)
			{
				physmem[i]->chance = 0;
 
			}

			else 
			{
				loc = i;
				break;
			}

		}

		
		physmem_evict(loc, type);  
		physmem_load(loc, pte, type);


	}


}



