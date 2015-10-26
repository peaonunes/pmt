#ifndef UTILS_H
#define UTILS_H

#include <vector>

struct program_args {
  program_args();
  ~program_args();

  int allowed_edit_distance;
  char* pattern_file;
  std::vector<std::string> patterns;
  bool kmp_flag;
  bool help_flag;
  char** source_text_files;
};

program_args get_program_parameters(int argc, char** argv);
void print_help_text();
int glob_error(const char *path, int eerrno);
void read_pattern_file(program_args &args);
void search_files(program_args &args);
int is_regular_file(const char *path);

#endif
