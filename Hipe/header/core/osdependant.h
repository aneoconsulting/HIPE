#pragma once


#ifdef WIN32
#define NO_EXCEPT
#else
#define NO_EXCEPT noexcept
#endif