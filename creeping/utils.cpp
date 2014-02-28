#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "utils.h"

void
utils_setenv(const char *name, const char *value, int overwrite)
{
	#ifdef _WIN32
	SetEnvironmentVariable(name, value);
	#else
	setenv(name, value, overwrite);
	#endif	
}

#ifdef _WIN32
int unsetenv(const char *name)
{
  size_t len;
  char **ep;

  if (name == NULL || *name == '\0' || strchr (name, '=') != NULL)
    {
      return -1;
    }

  len = strlen (name);

  ep = _environ;
  while (*ep != NULL)
    if (!strncmp (*ep, name, len) && (*ep)[len] == '=')
      {
    /* Found it.  Remove this pointer by moving later ones back.  */
    char **dp = ep;

    do
      dp[0] = dp[1];
    while (*dp++);
    /* Continue the loop in case NAME appears again.  */
      }
    else
      ++ep;

  return 0;
}
#endif
