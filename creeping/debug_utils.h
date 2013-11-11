#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#ifdef WITH_DEBUG_UTILS 
#define DEBUG_PRINT_LINE { fprintf( stderr, "%s ::: %s ::: <%d>:\n", __FILE__,__FUNCTION__, __LINE__ ); }
#else
#define DEBUG_PRINT_LINE {  }
#endif

#endif //DEBUG_UTILS_H
