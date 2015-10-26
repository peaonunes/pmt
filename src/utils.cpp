#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <glob.h>
#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "utils.h"
#include "search/ApproximateSearchStrategy.h"
#include "search/Sellers.h"
#include "search/BoyerMoore.h"
#include "search/ExactSearchStrategy.h"
#include "search/KnuthMorrisPratt.h"
#include "search/Occurrence.h"
#include "input/FileReader.h"
#include "search/AhoCorasick.h"

using namespace std;

program_args::program_args()
	: allowed_edit_distance(0),
		pattern_file(0),
		kmp_flag(false),
		help_flag(false),
		source_text_files(0) { }

program_args::~program_args() {

}

program_args get_program_parameters(int argc, char** argv) {
	int option_index;
	int current_parameter;
	program_args args;

	struct option long_options[] =
	{
		{"edit",    required_argument, 0, 'e'},
		{"kmp",     required_argument, 0, 'k'},
		{"pattern", required_argument, 0, 'p'},
		{"help",    no_argument,       0, 'h'},
		{0, 0, 0, 0}
	};

	while (1) {
		current_parameter = getopt_long(argc, argv, "e:kp:h", long_options, &option_index);

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
			case 'k':
			args.kmp_flag = true;
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
			exit(1);
		}
	}

	if (optind < argc) {
		/* Legacy text
		If pattern_flag is set, then the remaining arguments
		are target textfiles. Otherwise, the first one will be
		the pattern string*/
		if (!args.pattern_file) {
			args.patterns.push_back(argv[optind++]);
		}

		if (optind < argc) {
			int source_text_files_length = argc - optind + 2;
			int i = 0;

			args.source_text_files = new char*[source_text_files_length];

			while (optind < argc) {
				args.source_text_files[i++] = argv[optind++];
			}

			args.source_text_files[i] = 0;
		}
	}

	return args;
}

void print_help_line(char const *msg1, char const *msg2) {
	cout << left << setw(35) << msg1 << setw(45) << msg2 << endl;
}

void print_help_text() {
	cout << "Usage: pmt [options] [pattern] textfilepath [textfilepath ...]" << endl;
	cout << "Options:" << endl;
	print_help_line("  -e, --edit=<edit distance>", "Sets allowed edit distance for approximated text search");
	print_help_line("  -k, --kmp", "Uses the KMP algorithm instead of Boyer Moore for exact searchs");
	print_help_line("  -p, --pattern=<pattern file>","Specifies file from which the program should read the patterns to be used (each line of the file specifies a pattern)");
	print_help_line("  -h, --help","Shows this message");
	cout << endl << "  If a pattern file is not specified, the first argument given to pmt will be read as the only pattern to be searched for in the text file. Several source text files can be specified at the same time." << endl;
}

int is_regular_file(const char *path) {
		struct stat path_stat;
		stat(path, &path_stat);
		return S_ISREG(path_stat.st_mode);
}

/* globerr --- print error message for glob() */

int glob_error(const char *path, int eerrno) {
	fprintf(stderr, "pmt: %s: %s\n", path, strerror(eerrno));
	return 0; /* let glob() keep going */
}

void read_pattern_file(program_args &args) {
  FileReader fr(args.pattern_file);
  string buffer;

  while(fr.hasContent()) {
    buffer = fr.readLine();

    if (buffer.size()) {
      args.patterns.push_back(buffer);
    }
  }
}

/*
 * search_files --- searches source_text_files entries
 * whose name matches with one or more of the given
 * filenames
 */

void search_files(program_args &args) {
  int i;
  int flags = 0;
  glob_t results;
  int ret;

  for (i = 0; args.source_text_files[i]; i++) {
    ret = glob(args.source_text_files[i], flags, glob_error, & results);
    if (ret != 0) {
      fprintf(stderr, "pmt: problem with %s (%s)\n",
        args.source_text_files[i],
        (ret == GLOB_ABORTED ? "filesystem problem" :
         ret == GLOB_NOMATCH ? "no match of pattern" :
         ret == GLOB_NOSPACE ? "no dynamic memory" :
         "unknown problem"));
      // continues even if it spots a problem
    } else {
      for (int i = 0; i < results.gl_pathc; ++i) {
        // Check if it really is a file
        if (!is_regular_file(results.gl_pathv[i])) {
          cout << results.gl_pathv[i] << " isn't a regular file" << endl;
        } // else {
        //   cout << results.gl_pathv[i] << endl;
        // }

        // call search algorithm
        if (args.allowed_edit_distance) { // approximate search
          ApproximateSearchStrategy* searchStrategy = new Sellers(args.allowed_edit_distance);
          vector<Occurrence> result;

          for (int j = 0; j < args.patterns.size(); j++) {
            result = searchStrategy->search(args.patterns[j], results.gl_pathv[i]);

            if (!result.size()) {
              cout << "No occurrences found." << endl;
            }

            for (int k = 0; k < result.size(); k++) {
              cout << "Occurrence at line " << result[k].lineNumber <<
                ", ending at position " << result[k].position << " with error " << result[k].error << endl;
            }
          }

          delete searchStrategy;
        } else { // exact search
          if (args.patterns.size() > 1) {
            AhoCorasick ahoCorasick;
            vector<OccurrenceMultiplePatterns> result;

            result = ahoCorasick.search(args.patterns, results.gl_pathv[i]);

            if (!result.size()) {
              cout << "No occurrences found." << endl;
            }

            for (int j = 0; j < result.size(); j++) {
              printf ("%s: Occurrence for pattern %s at line %d starting at position %d \n", results.gl_pathv[i], result[j].value.c_str(), result[j].lineNumber, result[j].position);
              //cout << "Occurrence for pattern " << result[j].value <<
              //  " at line " << result[j].lineNumber <<
              //  ", starting at position " << result[j].position << endl;
            }
          } else {
            ExactSearchStrategy* searchStrategy;

            if (args.kmp_flag) {
              searchStrategy = new KnuthMorrisPratt();
            } else {
              searchStrategy = new BoyerMoore();
            }

            vector<Occurrence> result;

            result = searchStrategy->search(args.patterns[0], results.gl_pathv[i]);

            if (!result.size()) {
              cout << "No occurrences found." << endl;
            }

            for (int k = 0; k < result.size(); k++) {
              printf ("%s: Occurrence at line %d  starting at position %d \n", results.gl_pathv[i],result[k].lineNumber, result[k].position);
              //cout << "Occurrence at line " << result[k].lineNumber << ", starting at position " << result[k].position << endl;
            }

            delete searchStrategy;
          }
        }
      }
    }
  }
  globfree(&results);
}
