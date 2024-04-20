/*
While the std::shared_ptr has limited use in single-threaded code, in multi-threaded code, the notion of shared ownership (even if temporary) is fairly common.

However, while multiple instances of std::shared_ptr that point to the same memory can be managed safely in multi-threaded code, operations on a single instance of a std::shared_ptr are not thread-safe.

This is why C++20 introduced std::atomic<std::shared_ptr<T>> and std::atomic<std::weak_ptr<T>>, which can be used to implement thread-safe shared data structures.

Unfortunately, the GCC libstdc++ implementation is not lock-free (Clang libc++ doesn't implement it), so the resulting performance can be disappointing.
*/

#include <memory>
#include <atomic>
#include <optional>

template<typename T>
struct Stack
{
    struct Node
    {
        T value;
        std::shared_ptr<Node> prev;
    };

    void push(T value)
    {
        auto active = std::make_shared<Node>(std::move(value), head_.load());
        while(not head_.compare_exchange_weak(active->prev, active));
    }

    std::optional<T> pop()
    {
        auto active = head_.load();

        while(active != nullptr && not head_.compare_exchance_weak(active, active->prev));

        if(active != nullptr)
        {
            return { std::move(active->value)};
        }

        else
        {
            return std::nullopt;
        }
    }

    private:
    // thread-safe shared data structured
    std::atomic<std::shared_ptr<Node> > head_;
  
};
