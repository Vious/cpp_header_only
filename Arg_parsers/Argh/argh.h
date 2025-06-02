/* reference to https://github.com/adishavit/argh,
thanks for their contributions */
#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>

namespace argh
{

#if !defined(__GNUC__) || (__GNUC__ >= 5)
    using StringStream = std::istringstream;
#else
    
#endif
    
} // namespace argh

