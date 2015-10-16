#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include <glob.h>
#include <stdio.h>
#include <cstring>

#include "utils.h"

using namespace std;

program_args::program_args()
  : allowed_edit_distance(0),
    pattern_file(0),
    patterns(0),
    help_flag(false),
    source_text_files(0) { }

program_args get_program_parameters(int argc, char** argv) {
  int option_index;
  int current_parameter;
  program_args args;

  struct option long_options[] =
  {
    {"edit",    required_argument, 0, 'e'},
    {"pattern", required_argument, 0, 'p'},
    {"help",    no_argument,       0, 'h'},
    {0, 0, 0, 0}
  };

  while (1) {
    current_parameter = getopt_long(argc, argv, "e:p:h", long_options, &option_index);

    if (current_parameter == -1) {
      break;
    }

    switch (current_parameter) {
      case 0:
      // No momento, nenhum argumento está sendo usado para setar uma flag
      break;
      case 'e':
      args.allowed_edit_distance = atoi(optarg);
      break;
      case 'p':
      args.pattern_file = optarg;
      break;
      case 'h':
      args.help_flag = true;
      break;
      case '?':
      // Um argumento desconhecido é apenas ignorado no momento
      break;
      default:
      abort();
    }
  }

  if (optind < argc) {
    /* If pattern_flag is set, then the remaining arguments
    are target textfiles. Otherwise, the first one will be
    the pattern string*/
    if (!args.pattern_file) {
      args.patterns = new char*[2];
      args.patterns[0] = argv[optind++];
      args.patterns[1] = 0;
    }

    if (optind < argc) {
      int source_text_files_length = argc - optind + 2;
      int i = 0;

      args.source_text_files = new char*[source_text_files_length];

      while (optind < argc) {
        args.source_text_files[i++] = argv[optind++];
      }

      //args.source_text_files[i] = 0;
      
      args.source_text_files = match_files(i, args.source_text_files);

    }
  }

  return args;
}

void print_help_text() {
  cout << "Usage: pmt [options] [pattern] <textfilepath> [moretextfiles]" << endl;
  cout << "Options:" << endl;
  cout << "  -e <edit distance>\t\tSets allowed edit distance for approximated text search" << endl;
  cout << "  --edit=<edit distance>\tSame as previous option" << endl;
  cout << "  -p <pattern file>\t\tSpecifies file from which the program should read the patterns to be used (each line of the file specifies a pattern)" << endl;
  cout << "  --pattern=<pattern file>\tSame as previous option" << endl;
  cout << "  -h\t\t\t\tShows this message" << endl;
  cout << "  --help\t\t\tSame as previous option" << endl;
  cout << endl << "  If a pattern file is not specified, the first argument given to pmt will be read as the only pattern to be searched for in the text file. Several source text files can be specified at the same time." << endl;
}

/* globerr --- print error message for glob() */

int glob_error(const char *path, int eerrno)
{
  fprintf(stderr, "pmt: %s: %s\n", path, strerror(eerrno));
  return 0; /* let glob() keep going */
}

/* 
 * match_files --- updates source_text_files with every file
 * whose name matches with one or more of the given filenames
 */

char** match_files(int files_index, char **source_text_files)
{
  int i;
  int flags = 0;
  glob_t results;
  int ret;
  int results_size;
  char** resulting_text_files;

  for (i = 0; i < files_index; i++) {
    flags |= (i > 0 ? GLOB_APPEND : 0);
    ret = glob(source_text_files[i], flags, glob_error, & results);
    if (ret != 0) {
      fprintf(stderr, "pmt: problem with %s (%s), stopping early\n",
        source_text_files[i],
        (ret == GLOB_ABORTED ? "filesystem problem" :
         ret == GLOB_NOMATCH ? "no match of pattern" :
         ret == GLOB_NOSPACE ? "no dynamic memory" :
         "unknown problem"));
      break;
    }
  }
  
  results_size = results.gl_pathc+1;
  resulting_text_files = new char *[results_size];

  for (i = 0; i < results_size-1; i++) {
    strcpy(resulting_text_files[i], results.gl_pathv[i]);
    printf("%s - %s\n", results.gl_pathv[i], resulting_text_files[i]);
  }

  resulting_text_files[results_size-1] = '\0';

  globfree(& results);

  for (i = 0; i < results_size-1; i++) {
    printf("%s\n", resulting_text_files[i]);
  }

  return resulting_text_files;
}