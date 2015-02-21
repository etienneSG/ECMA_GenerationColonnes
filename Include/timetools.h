//==============================================================================
//
// To get CPU and user time.
// - double get_wall_time() return the user time.
// - double get_cpu_time() return the CPU time.
//
//==============================================================================


#ifndef TIMETOOLS_H
#define TIMETOOLS_H


// Windows
#ifdef _WIN32
#include <Windows.h>

/** Return user time */
double get_wall_time();

/** Return CPU time */
double get_cpu_time();

#endif

//  Posix/Linux
#ifdef __linux__
#include <sys/time.h>
#include <time.h>

/** Return user time */
double get_wall_time();

/** Return CPU time */
double get_cpu_time();

#endif


#endif



