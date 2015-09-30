// Generated by Neo

#ifndef _COMMONS_PILE_BUFFER_H_
#define _COMMONS_PILE_BUFFER_H_

#include <commons/defines.h>

#ifdef _POSIX_
#include <pthread.h>
#else
#error without macro `_POSIX_`
#endif

#include <commons/types.h>

namespace commons {
class PileBuffer {
   public:
    PileBuffer(size_t capacity, pile_buffer_cb cb, void* user,
               void* next_pile_ptr = nullptr);
    virtual ~PileBuffer();

    bool append(const void* data, size_t size,
                const void* additional = nullptr);

   private:
    size_t capacity_;
    pile_buffer_cb cb_;
    void* user_;

    size_t left_;
    size_t index_;

    uint8_t* buffer_;
    bool is_buffer_from_outer;

#ifdef _POSIX_
    pthread_mutex_t* mutex_;
#endif

    ONLY_CUSTOM_CONSTRUCTION(PileBuffer);
};
} /* namespace: commons */

#endif /* _COMMONS_PILE_BUFFER_H_ */