#ifndef UTILS_H
#define UTILS_H

#include <string>

#ifdef _WIN32
#define sleep Sleep
int unsetenv(const char *name);
#endif

void utils_make_tmp_file_path(std::string * ret, const char * filename);
void utils_setenv(const char *name, const char *value, int overwrite);
void utils_net_dowmload_data(const char * url, std::string * data);
void utils_net_dowmload_file(const char * url, const char * path);
#endif
