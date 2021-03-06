// Generated by Neo

#ifndef _COMMONS_BLOCKED_QUEUE_H_
#define _COMMONS_BLOCKED_QUEUE_H_

#include <commons/defines.h>

#if !(defined(_COCOA_PLATFORM_) || defined(_ANDROID_PLATFORM_) || \
      defined(_POSIX_))
#error PLATFORM must be specific
#endif

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include <queue>

#include <commons/types.h>
#include <commons/commons.h>

namespace commons {
template <typename T>
class BlockedQueue {
   public:
    BlockedQueue(blocked_queue_clean_cb cb);
    virtual ~BlockedQueue();

    size_t left();
    void clean();

    const T* head();
    T* poll();
    void offer(T* t);

    // [Neo] sem post, can poll a nullptr as fin
    void fin();

   private:
    std::queue<T*>* queue_;

    char* sem_name_;
    sem_t* semaphore_;
    pthread_mutex_t* mutex_;

    blocked_queue_clean_cb cb_;

    ONLY_CUSTOM_CONSTRUCTION(BlockedQueue);
};

template <typename T>
BlockedQueue<T>::BlockedQueue(blocked_queue_clean_cb cb) {
    cb_ = cb;
    queue_ = new std::queue<T*>;

    mutex_ = new pthread_mutex_t;
    pthread_mutex_init(mutex_, nullptr);

#ifdef _COCOA_PLATFORM_
    sem_name_ = new char[20];
    sprintf(sem_name_, "%" PRIu64 "%02X", current_ts(), random_range());
    semaphore_ = sem_open(sem_name_, kSemFlag, kSemMode, kSemValue);
#else
    sem_name_ = nullptr;
    semaphore_ = new sem_t;
    sem_init(semaphore_, 0, 0);
#endif

    if (SEM_FAILED == semaphore_) {
        LOGE("Error: BQ open sem failed: %d!\n", errno);
    }
}

template <typename T>
BlockedQueue<T>::~BlockedQueue() {
    if (SEM_FAILED != semaphore_) {
        sem_post(semaphore_);

#ifdef _COCOA_PLATFORM_
        if (nullptr != sem_name_) {
            sem_unlink(sem_name_);
            delete[] sem_name_;
            sem_name_ = nullptr;
        }
        sem_close(semaphore_);
#else
        sem_destroy(semaphore_);
        delete semaphore_;
#endif

        semaphore_ = SEM_FAILED;
    }

    if (nullptr != mutex_) {
        pthread_mutex_destroy(mutex_);
        delete mutex_;
        mutex_ = nullptr;
    }

    if (nullptr != queue_) {
        while (queue_->size() > 0) {
            T* t = queue_->front();
            queue_->pop();
            if (nullptr != cb_) {
                cb_(t);
            }
        }

        delete queue_;
        queue_ = nullptr;
    }
}

template <typename T>
const T* BlockedQueue<T>::head() {
    T* t = nullptr;

    pthread_mutex_lock(mutex_);
    if (queue_->size() > 0) {
        t = queue_->front();
    }
    pthread_mutex_unlock(mutex_);

    return t;
}

template <typename T>
T* BlockedQueue<T>::poll() {
    T* t = nullptr;

    if (SEM_FAILED != semaphore_) {
        sem_wait(semaphore_);
    }

    pthread_mutex_lock(mutex_);
    if (queue_->size() > 0) {
        t = queue_->front();
        queue_->pop();
    }
    pthread_mutex_unlock(mutex_);

    return t;
}

template <typename T>
void BlockedQueue<T>::offer(T* t) {
    if (nullptr != t) {
        pthread_mutex_lock(mutex_);
        queue_->push(t);
        pthread_mutex_unlock(mutex_);

        if (SEM_FAILED != semaphore_) {
            sem_post(semaphore_);
        }
    }
}

template <typename T>
size_t BlockedQueue<T>::left() {
    size_t result = 0;

    pthread_mutex_lock(mutex_);
    result = queue_->size();
    pthread_mutex_unlock(mutex_);

    return result;
}

template <typename T>
void BlockedQueue<T>::fin() {
    if (SEM_FAILED != semaphore_) {
        sem_post(semaphore_);
    }
}

template <typename T>
void BlockedQueue<T>::clean() {
    pthread_mutex_lock(mutex_);
    while (queue_->size() > 0) {
        T* t = queue_->front();
        queue_->pop();
        if (nullptr != cb_) {
            cb_(t);
        }
    }
    pthread_mutex_unlock(mutex_);
}
} /* namespace: commons */

#endif /* _COMMONS_BLOCKED_QUEUE_H_ */
