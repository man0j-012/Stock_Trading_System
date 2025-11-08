#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <typename T>
class ThreadSafeQueue {
public:
    void push(T value) {
        {
            std::scoped_lock lock(m_);
            q_.push(std::move(value));
        }
        cv_.notify_one();
    }

    std::optional<T> pop() {
        std::unique_lock lock(m_);
        cv_.wait(lock, [&]{ return !q_.empty() || stopped_; });
        if (q_.empty()) return std::nullopt;
        T v = std::move(q_.front());
        q_.pop();
        return v;
    }

    void stop() {
        {
            std::scoped_lock lock(m_);
            stopped_ = true;
        }
        cv_.notify_all();
    }

private:
    std::queue<T> q_;
    std::mutex m_;
    std::condition_variable cv_;
    bool stopped_{false};
};
