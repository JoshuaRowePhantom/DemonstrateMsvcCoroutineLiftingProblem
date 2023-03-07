// DemonstrateCoroutineLiftingProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <coroutine>
#include <iostream>
#include "Windows.h"

struct future
{
    struct promise_type
    {
        void* operator new(size_t size)
        {
            return ::VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        }

        void operator delete(void* value, size_t size)
        {
            ::VirtualFree(value, 0, MEM_RELEASE);
        }

        std::suspend_never initial_suspend() { return {}; };
        std::suspend_never final_suspend() noexcept { return {}; };
        void unhandled_exception() {}
        void return_void() {}

        auto get_return_object() { return future{}; }
    };
};

struct bool_awaitable
{
    bool await_ready() const noexcept
    {
        return false;
    }

    bool await_suspend(
        std::coroutine_handle<future::promise_type> caller
    )
    {
        caller.destroy();
        return true;
    }

    void await_resume()
    {
    }
};

struct symmetric_transfer_awaitable
{
    bool await_ready() const noexcept
    {
        return false;
    }

    auto await_suspend(
        std::coroutine_handle<future::promise_type> caller
    )
    {
        caller.destroy();
        return std::noop_coroutine();
    }

    void await_resume()
    {
    }
};

future my_coroutine1()
{
    co_await bool_awaitable{};
}

future my_coroutine2()
{
    co_await symmetric_transfer_awaitable{};
}

int main()
{
    my_coroutine1();
    my_coroutine2();
    return 0;
}
