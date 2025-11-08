#include "ThreadPool.hpp"

ThreadPool::ThreadPool(std::size_t n) {
    if (n == 0) n = 1;
    threads_.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        threads_.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::scoped_lock lock(m_);
        stop_ = true;
    }
    cv_.notify_all();
    for (auto& t : threads_) if (t.joinable()) t.join();
}

void ThreadPool::worker() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock lock(m_);
            cv_.wait(lock, [&]{ return stop_ || !jobs_.empty(); });
            if (stop_ && jobs_.empty()) return;
            job = std::move(jobs_.front());
            jobs_.pop();
        }
        job();
    }
}
