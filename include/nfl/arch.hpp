#ifndef NFL_ARCH_HPP
// #define NFL_OPTIMIZED
// #define __AVX2__
// #define NTT_AVX2
// -D__AVX2__ -DNTT_AVX2 -DNFL_OPTIMIZED
#include "nfl/arch/common.hpp"

#ifdef NFL_OPTIMIZED

#if defined __AVX2__ && defined NTT_AVX2
#define CC_SIMD nfl::simd::avx2
#elif defined __SSE4_2__ && defined NTT_SSE
#define CC_SIMD nfl::simd::sse
#endif

#endif

#ifndef CC_SIMD
#define CC_SIMD nfl::simd::serial
#endif

#endif
