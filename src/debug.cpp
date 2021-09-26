#include "debug.h"
#include <sstream>

using namespace std;

stringstream debugLog;

template <typename... T>
void appendDebugLog(const T&... args)
{
	((debugLog << args), ...);
}