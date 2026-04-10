#ifndef thread_pool_h
#define thread_pool_h

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    ThreadPool(size_t threadCount);
    ~ThreadPool();

    template<typename F>
    auto enqueue(F&& f) -> std::future<decltype(f())>;

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex mutex;
    std::condition_variable condition;
    bool stop = false;
};

#include "ThreadPool.inl"

#endif // thread_pool_h
