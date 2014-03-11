#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "utils.h"


void 
utils_make_tmp_file_path(std::string * ret, const char * filename)
{
	std::string tmpdir;
#ifdef _WIN32
	tmpdir.resize(261,0x0);
	GetTempPath(tmpdir.capacity(), &tmpdir[0]);
#else
	tmpdir = "/tmp/";
#endif
	if(ret == NULL)
		return;
	
	*ret = tmpdir.c_str();
	
	if(filename != NULL)
		*ret += filename;
}

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

size_t write_data(void *ptr, size_t size, size_t nmemb, std::string * data) {
    data->append((const char*)ptr, size * nmemb);
    return nmemb;
}

void utils_net_dowmload_data(const char * url, std::string * data)
{
	CURL * curl = curl_easy_init();
	if (curl) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
}
