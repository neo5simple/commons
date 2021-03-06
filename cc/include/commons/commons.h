// Generated by Neo

#ifndef _COMMONS_H_
#define _COMMONS_H_

#define __STDC_FORMAT_MACROS
#include <stddef.h>
#include <inttypes.h>

#include <stdlib.h>
#include <time.h>

#include <commons/defines.h>

namespace commons {
#ifdef _COCOA_PLATFORM_
extern const int kSemMode;
extern const int kSemValue;
extern const int kSemFlag;
#endif /* _COCOA_PLATFORM_ */

uint64_t current_ts();
void msleep(size_t millis);

char* cat(const char* pre, const char* sub);

inline void do_nothing(void*) {}

inline void init_rand() { srand(static_cast<unsigned int>(time(nullptr))); }
inline int random_range(int min = 0, int max = 0xFF) {
    return (min + rand() % (max - min + 1));
}

inline bool is_range(int input, int from, int to) {
    return (input >= from && input <= to);
}
inline bool is_range_ts(uint64_t input, uint64_t base, int adjust) {
    return (input >= base - adjust && input <= base + adjust);
}

inline bool is_power_of_2(size_t x) { return !(x & (x - 1)); }
inline size_t next_pow_of_2(size_t x) {
    if (false == is_power_of_2(x)) {
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
#if (defined(__x86_64__) || defined(__aarch64__))
        x |= x >> 32;
#endif
        x = x + 1;
    }
    return x;
}
} /* namespace: commons */

#endif /* _UTILS_COMMONS_H_ */
