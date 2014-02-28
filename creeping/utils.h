#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
#define sleep Sleep
int unsetenv(const char *name);
#endif

void utils_setenv(const char *name, const char *value, int overwrite);

#endif
