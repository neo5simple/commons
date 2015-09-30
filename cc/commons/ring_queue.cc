// Generated by Neo

#include <commons/ring_queue.h>

#include <commons/commons.h>
#include <commons/blocked_queue.h>

namespace commons {
void* ring_dequeued(void* handle) {
    RingQueue* queue = reinterpret_cast<RingQueue*>(handle);

    do {
        void* data = queue->queue_->poll();
        if (nullptr == data) {
            break;
        }

        if (nullptr != queue->cb_) {
            queue->cb_(queue->user_, queue, data);
        }
    } while (true);

    queue->pid_ = 0;
    return nullptr;
}

RingQueue::RingQueue(int capacity, ring_queue_cb cb, void* user) {
    cb_ = cb;
    user_ = user;

    head_ = 0;
    tail_ = 0;
    offset_ = 0;
    capacity_ = capacity;

    pid_ = 0;
    pdata_ = nullptr;

    if (capacity_ > 1 && nullptr != cb_) {
        mutex_ = new pthread_mutex_t;
        pthread_mutex_init(mutex_, nullptr);

        pdata_ = new void*[capacity_];
        for (size_t i = 0; i < capacity_; ++i) {
            pdata_[i] = nullptr;
        }

        // need full enqueued?
        // offset_ = capacity_;

        queue_ = new BlockedQueue<void>(do_nothing);

        pthread_create(&pid_, nullptr, ring_dequeued, this);
        if (0 == pid_) {
            LOGE("Error: RQ internal error\n");
        }

    } else {
        LOGE("Error: RQ open sem failed: %d!\n", errno);
    }
}
RingQueue::~RingQueue() {
    cb_ = nullptr;
    user_ = nullptr;

    if (0 != pid_) {
        queue_->fin();
        pthread_join(pid_, nullptr);
        delete queue_;
        queue_ = nullptr;
    }

    if (nullptr != mutex_) {
        pthread_mutex_destroy(mutex_);
        delete mutex_;
        mutex_ = nullptr;
    }

    if (nullptr != pdata_) {
        delete[] pdata_;
        pdata_ = nullptr;
    }
}

const void* RingQueue::head() {
    void* data = nullptr;

    pthread_mutex_lock(mutex_);
    if (offset_ > 0 && nullptr != pdata_[head_]) {
        data = pdata_[head_];
    }
    pthread_mutex_unlock(mutex_);

    return data;
}
const void* RingQueue::dequeue() {
    void* data = nullptr;

    if (nullptr != cb_ && nullptr != pdata_) {
        pthread_mutex_lock(mutex_);
        if (offset_ > 0 && nullptr != pdata_[head_]) {
            data = pdata_[head_];

            ++head_;
            --offset_;

            if (head_ >= capacity_) {
                head_ = 0;
            }

            if (nullptr != queue_) {
                queue_->offer(data);
            }
        }
        pthread_mutex_unlock(mutex_);
    }

    return data;
}
bool RingQueue::enqueue(const void* data) {
    bool result = false;

    if (nullptr != cb_ && nullptr != pdata_ && nullptr != data) {
        pthread_mutex_lock(mutex_);
        if (offset_ < capacity_) {
            pdata_[tail_] = const_cast<void*>(data);

            ++tail_;
            ++offset_;

            if (tail_ >= capacity_) {
                tail_ = 0;
            }

            result = true;
        }
        pthread_mutex_unlock(mutex_);
    }

    return result;
}
} /* namespace: commons */
