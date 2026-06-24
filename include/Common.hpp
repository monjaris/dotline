#pragma once

#include <cstdint>

#define DOTL_NAMESPACE_BEGIN(_namespace_name)  namespace _namespace_name {
#define DOTL_NAMESPACE_END()    };


using usize = std::size_t;
using isize = __ssize_t;
using int32 = int32_t;
using int64 = int64_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using fp32 = float;
using fp64 = double;

