#pragma once

#ifndef DELETE_PTR
#define DELETE_PTR(p)	if (p) { delete p; p = NULL; }
#endif

#ifndef DELETE_PTRS
#define DELETE_PTRS(p)	if (p) { delete [] p; p = NULL; }
#endif

#ifndef CLOSE_HANDLE
#define CLOSE_HANDLE(handle) if (handle) { ::CloseHandle((HANDLE)handle); handle = NULL; }
#endif

#include <string>
namespace eric { namespace common {

extern void GetLastErrorMessage(std::string &strMessage, unsigned long dwErrorCode);
extern std::string GetLastErrorMessage(unsigned long dwErrorCode);

}}
