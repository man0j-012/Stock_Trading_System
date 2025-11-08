#pragma once
#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t n);
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using Ret = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<Ret()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<Ret> res = task->get_future();
        {
            std::unique_lock lock(m_);
            if (stop_) throw std::runtime_error("enqueue on stopped ThreadPool");
            jobs_.emplace([task](){ (*task)(); });
        }
        cv_.notify_one();
        return res;
    }

private:
    void worker();

    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> jobs_;
    std::mutex m_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{false};
};
