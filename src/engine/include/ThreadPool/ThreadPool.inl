#ifndef thread_pool_inl
#define thread_pool_inl

#include "ThreadPool.h"

inline ThreadPool::ThreadPool(size_t threadCount)
{
    for (size_t i = 0; i < threadCount; ++i) this->workers.emplace_back([this]()
    {
        while (true)
        {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(this->mutex);

                this->condition.wait(lock, [this]()
                { return this->stop || !this->tasks.empty(); });

                if (this->stop && this->tasks.empty()) return;

                task = std::move(this->tasks.front());
                this->tasks.pop();
            }

            task();
        }
    });
    
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->stop = true;
    }

    this->condition.notify_all();

    for (auto& worker : this->workers) worker.join();
}

template<typename F>
auto ThreadPool::enqueue(F&& f) -> std::future<decltype(f())>
{
    using ReturnType = decltype(f());

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::forward<F>(f));

    std::future<ReturnType> result = task->get_future();

    {
        std::unique_lock<std::mutex> lock(this->mutex);

        this->tasks.emplace([task]() { (*task)(); });
    }

    this->condition.notify_one();

    return result;
}

#endif // thread_pool_inl
