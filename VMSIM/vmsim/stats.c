//#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <stats.h>
#include <options.h>

stats_t *stats;

void stats_output_type(FILE *o, type_count_t output, const char *label);

void stats_init() {
  stats = (stats_t*)calloc(1, sizeof(stats_t));
  assert(stats);
  if (opts.output_file) {
    stats->output = fopen(opts.output_file, "a+");
    if (stats->output == NULL) {
      perror("vmsim: unable to open output file for write");
      abort();
    }
  } else {
    stats->output = stdout;
  }
}

void stats_output() {
  FILE *o = stats->output;
  fprintf(o, "\n\n Simulation Parameters:"); 
  fprintf(o, "\n    phys_pages, pagesize, input_file, fault_handler, ref_limit\n");
  fprintf(o, "     %d,  %d,  %s,  %s,  %ld\n", opts.phys_pages, opts.pagesize,
	  (opts.input_file ? opts.input_file : "stdin"),
	  opts.fault_handler->name, opts.limit);
  
  fprintf(o, "\n Simulation Results:"); 
  fprintf(o, "\n\tStat Type: code,load,store;   total\n");
  stats_output_type(o, stats->references, "Memory references");
  stats_output_type(o, stats->miss, "Page Faults");
  stats_output_type(o, stats->compulsory, "Compulsory Page Faults");
  stats_output_type(o, stats->evict_dirty, "(Dirty) Page Writes");

  fclose(o);
  stats->output = NULL;
}

void stats_output_type(FILE* o, type_count_t output, const char *label) {
  fprintf(o, "\t%s: %u,%u,%u;  %u\n", label, output[REF_KIND_CODE],
	 output[REF_KIND_LOAD], output[REF_KIND_STORE], 
	  output[REF_KIND_CODE]+ output[REF_KIND_LOAD]+
	  output[REF_KIND_STORE]);
}
