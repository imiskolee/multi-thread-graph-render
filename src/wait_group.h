//
// Created by imisk on 25-6-23.
//

#ifndef WAIT_GROUP_H
#define WAIT_GROUP_H

#include <mutex>
#include <condition_variable>
#include <atomic>

class WaitGroup {
public:
    // 添加n个待等待的任务
    void add(int n) {
        count_.fetch_add(n, std::memory_order_relaxed);
    }

    // 标记一个任务已完成
    void done() {
        if (count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            std::unique_lock<std::mutex> lock(mutex_);
            count_zero_.notify_all();
        }
    }

    // 等待所有任务完成
    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        count_zero_.wait(lock, [this] { return count_.load(std::memory_order_relaxed) == 0; });
    }

private:
    std::atomic<int> count_{0};
    std::mutex mutex_;
    std::condition_variable count_zero_;
};

#endif //WAIT_GROUP_H
