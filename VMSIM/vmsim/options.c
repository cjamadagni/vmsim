/*
 * options.c - Process command line options, initializing the global
 *             options struct.
 */

//#include <config.h>

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vmsim.h>
#include <options.h>
#include <fault.h>
#include <util.h>

#define MIN_PHYS_PAGES 3
#define MIN_PAGESIZE 16

/* Global options structure. process_options will set it's values */
opts_t opts;

static const char *shortopts = "hvtVp:s:l:o:";

/**********************************************************************/
/* Handle systems without GNU libc-style longopt support              */
 
#ifdef HAVE_GETOPT_LONG

#define __GNU_SOURCE
#include <getopt.h>
static const struct option longopts[] =
{ { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'V' },
  { "verbose", no_argument, NULL, 'v' },
  { "test", no_argument, NULL, 't' },  
  { "output", required_argument, NULL, 'o' },
  { "limit", required_argument, NULL, 'l' },
  { "pages", required_argument, NULL, 'p' },
  { "size", required_argument, NULL, 's' },    
  { 0, 0, 0, 0 }
};

#define GETOPT(argc, argv) getopt_long(argc, argv, shortopts, longopts, NULL)
#else
#define GETOPT(argc, argv) getopt(argc, argv, shortopts)
#endif
/**********************************************************************/

static void options_handle_algorithm(const char *alg_name);
static long options_atoi(const char *arg);
static void options_print_help();
static void options_print_version();
static char *_longopt(char *longopt_help);
static char *_zlibhelp();
static void _algorithm_help();

/* Process the argc/argv array, updating the global
 * options struct 'opts'. */
void options_process(int argc, char **argv) {
  int opt;
  /* Options handled within this function: */
  int help = FALSE, version = FALSE;

  opts.output_file = NULL;
  opts.input_file = NULL;
  opts.verbose = FALSE;
  opts.test = FALSE;
  opts.pagesize = 1024;
  opts.phys_pages = 128;
  opts.limit = 0;
  
  while (1) {
    opt = GETOPT(argc, argv);
    if (opt == -1)
      break;
    switch (opt) {
    case 'h':
      help = TRUE;
      break;
    case 'v':
      opts.verbose = TRUE;
      break;
    case 'o':
      opts.output_file = optarg;
      break;
    case 'V':
      version = TRUE;
      break;
    case 'l':
      opts.limit = options_atoi(optarg);
      break;
    case 't':
      opts.test = TRUE;
      break;
    case 'p':
      opts.phys_pages = options_atoi(optarg);
      break;
    case 's':
      opts.pagesize = options_atoi(optarg);
      break;
    case '?':
      /* Unrecognized option - print usage */
      help = TRUE;
      break;
    default:
      printf("getopt returned '%c' - unhandled option\n", opt);
      abort();
    }
  }

  if (version) {
    options_print_version();
    if (help) printf("\n");
  }
  if (help) {
    options_print_help();
    exit(0);
  }

  if (opts.limit < 0) {
    fprintf(stderr, "vmsim: limit must be > 0\n");
    exit(1);
  }

  if (opts.phys_pages < MIN_PHYS_PAGES) {
    fprintf(stderr, "vmsim: must have at least %d pages\n", MIN_PHYS_PAGES);
    exit(1);
  }

  if (opts.pagesize < MIN_PAGESIZE) {
    fprintf(stderr, "vmsim: pagesize must be at least %d bytes\n", MIN_PAGESIZE);
    exit(1);
  }
  if (log_2(opts.pagesize) == -1) {
    fprintf(stderr, "vmsim: pagesize must be a power of 2\n");
    exit(1);
  }

/*The variable optind is the index of the next element of the argv[] vector to be processed. It shall be initialized to 1 by the system, and getopt() shall update it when it finishes with each element of argv[]. http://linux.die.net/man/3/optind */

  if (optind >= argc) {
    fprintf(stderr, "vmsim: algorithm must be specified\n");
    exit(1);
  }
  options_handle_algorithm(argv[optind]);
  
  if (optind+1 < argc) {
    opts.input_file = argv[optind+1];
  }

  if (opts.input_file == NULL ||
      strcmp("-", opts.input_file) == 0) {
    opts.input_file = NULL;
    if (opts.verbose) {
      printf("vmsim: reading from stdin\n");
    }
  } else if (opts.verbose) {
    printf("vmsim: reading from %s\n", opts.input_file);
  }

  if (opts.verbose && opts.output_file) {
    printf("vmsim: sending output to %s\n", opts.output_file);
  }
}

long options_atoi(const char *arg) {
  char *end;
  long ret;
  ret = strtol(arg, &end, 10);
  if (*end != '\0') {
    fprintf(stderr, "vmsim: invalid integer argument: %s\n", arg);
    exit(1);
  }
  return ret;
}

void options_handle_algorithm(const char *alg_name) {
  fault_handler_info_t *alg;
  for (alg = fault_handlers; alg->name != NULL; alg++) {
    if (strcmp(alg->name, alg_name) == 0) {
      break;
    }
  }
  if (alg->name == NULL) {
    fprintf(stderr, "vmsim: no algorithm named '%s' available\n", alg_name);
    exit(1);
  } else if (opts.verbose) {
    printf("vmsim: using replacement algorithm '%s'\n", alg_name);
  }
  opts.fault_handler = alg;
}

void options_print_version() {
  printf("Package version 1.01 \n");
  printf("Report bugs to rick@cs.washington.edu\n");
  printf("Available under the GNU General Public License\n");
}

void options_print_help() {
  printf("Usage: vmsim [OPTIONS] ALGORITHM [TRACEFILE|-]\n");
  printf("Process TRACEFILE, simulating a VM system. Reports stats on paging behavior.\n");
  printf("If TRACEFILE is not specified or is '-', input will be taken from stdin.\n");
  printf("\n");
  printf("ALGORITHM specifies the fault handler, and should be one of:\n");
  _algorithm_help();
  printf("\n");
  printf("Options:\n");
  printf("-h%s               Print this message and exit.\n", _longopt("|--help"));
  printf("-V%s            Print the version information.\n", _longopt("|--version"));
  printf("-v%s            Verbose output. Includes progress output.\n", _longopt("|--verbose"));
  printf("-t%s               Run self tests.\n", _longopt("|--test"));
  printf("-o FILE%s   Append statistics output to the given file.\n", _longopt("|--output=FILE"));
  printf("-l REFS%s    Stop after first REFS refs.\n", _longopt("|--limit=REFS"));
  printf("-p PAGES%s  Simulate PAGES physical pages.\n", _longopt("|--pages=PAGES"));
  printf("                        Minimum value %d.\n", MIN_PHYS_PAGES);
  printf("-s SIZE%s     Simulate a page size of SIZE bytes.\n", _longopt("|--size=SIZE"));  
  printf("                        Size must be a power of 2.\n");
  
}

char *_zlibhelp() {
return "";
#ifdef HAVE_LIBZ
  return "Zlib is available; input may be compressed.\n";
#else
  return "Zlib not available; input must be decompressed.\nInstall Zlib (www.zlib.org) and rerun ./configure to add support.\n";
#endif
}

void _algorithm_help() {
  fault_handler_info_t *alg;
  printf("   ");
  for (alg = fault_handlers; alg->name != NULL; alg++) {
    printf("%s%s", alg->name, (alg+1)->name ? ", " : "\n");
  }
}

/* Helper function to handle help for case where we don't support GNU-style
 * longoptions.
 */
char *_longopt(char *longopt_help) {
#ifdef HAVE_GETOPT_LONG
  return longopt_help;
#else  
  char *i, *spaces = strdup(longopt_help);
  for (i = spaces; *i; i++) *i = ' ';
  return spaces;
#endif
}
