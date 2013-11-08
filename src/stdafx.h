// Ŭnicode please 
#define WIN32_LEAN_AND_MEAN
#define SR_USE_PCH 1
#define NOMINMAX 

#include "Kernel/OVR_Types.h"

#if SR_USE_PCH
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <climits>
#include <cstdarg>
#include <cstring>
#include <cstdint>
#include <cfloat>

#include <typeinfo>

#include <locale>   // wstring_convert
#include <codecvt>  // codecvt_utf8
#include <iostream> // cout
#include <string>   // stoi and u32string

#include <sstream>
#include <random>

#include <array>
#include <list>
#include <tuple>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <memory>
#include <deque>
#include <exception>

#include <unordered_map>
#include <unordered_set>

#include <type_traits>

// system
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*** NOTE: OVR.h is moved up above the #defs that disable OVR_DEFINE_NEW ***/
// libovr
#include <OVR.h>

//disable oculus sdk new
#ifdef OVR_DEFINE_NEW
#undef OVR_DEFINE_NEW
#define OVR_DEFINE_NEW new
#endif

#ifdef OVR_OS_WIN32
#include <windows.h>
#endif

// ogre
#include <Ogre.h>

#include "Gorilla.h"

#include "OIS.h"

#else
#error "NEED PCH"
#endif