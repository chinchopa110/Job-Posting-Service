#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;

    size_t size() const { return workers_.size(); }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
};

inline ThreadPool::ThreadPool(size_t numThreads) : stop_(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back([this] {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    condition_.wait(lock, [this] { return stop_.load() || !tasks_.empty(); });
                    if (stop_.load() && tasks_.empty()) return;
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    stop_.store(true);
    condition_.notify_all();
    for (auto& w : workers_) {
        if (w.joinable()) w.join();
    }
}

template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
    using return_type = typename std::invoke_result<F, Args...>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> result = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (stop_.load()) throw std::runtime_error("submit on stopped ThreadPool");
        tasks_.emplace([task]() { (*task)(); });
    }
    condition_.notify_one();
    return result;
}
