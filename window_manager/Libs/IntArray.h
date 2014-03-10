#ifndef _INT_ARRAYF_H_
#define _INT_ARRAYF_H_

#include <vector>

typedef std::vector<int> INT_ARRAY;

void StrToIntArray(INT_ARRAY &array, LPCTSTR str);
BOOL IsIntPresent(INT_ARRAY &array, int Num);

#endif // _INT_ARRAYF_H_