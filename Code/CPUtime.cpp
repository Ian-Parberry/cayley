/// \file CPUtime.cpp
/// \brief Cross-platform code for getting CPU time.

#include <cinttypes>

#ifdef _MSC_VER //Windows Visual Studio 

#include <windows.h>

/// Get CPU time in units of 100 nanoseconds, which is the default.
/// \return User CPU time in hundreds of nanoseconds.

uint64_t CPUTimeInCentiNanoseconds(){  
  uint64_t result = 0; //return result
  HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());

  if(h){ //got process handle successfully
    FILETIME ftCreation, ftExit, ftKernel, ftUser; //we only need ftUser

    if(GetProcessTimes(h, &ftCreation, &ftExit, &ftKernel, &ftUser)){
      ULARGE_INTEGER t; //ULARGE_INTEGER avoids alignment problems in Win64
      t.HighPart = ftUser.dwHighDateTime; //copy over ftUser to t
      t.LowPart = ftUser.dwLowDateTime; //copy over ftUser to t

      result = t.QuadPart; //this apparently avoids endian problems
    } //if

    CloseHandle(h); //done with process handle
  } //if

  return result;
} //CPUTimeInCentiNanoseconds

/// Get CPU time in nanoseconds.
/// \return User CPU time in nanoseconds.

uint64_t CPUTimeInNanoseconds(){  
  return 100LL*CPUTimeInCentiNanoseconds();
} //CPUTimeInNanoseconds

#else//other OS

#include <time.h>
  
/// Get CPU time in nanoseconds.
/// \return User CPU time in nanoseconds.

uint64_t CPUTimeInNanoseconds(){  
  return clock()*(1000000000LL/CLOCKS_PER_SEC);
} //CPUTimeInNanoseconds

#endif
