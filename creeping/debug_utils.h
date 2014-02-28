#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#ifdef WITH_DEBUG_UTILS 
#include "stdio.h"
#define DEBUG_PRINT_LINE { printf(  "%s ::: %s ::: <%d>:\n", __FILE__,__FUNCTION__, __LINE__ ); }
#else
#define DEBUG_PRINT_LINE {  }
#endif

#endif //DEBUG_UTILS_H
