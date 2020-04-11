#pragma once

#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace detail {

// simulate CPU intensive task
inline std::unique_ptr<std::vector<char>> do_expensive_work(std::string const& name, bool louder)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::string str = "...threads are busy async bees...hello " + name;
    std::unique_ptr<std::vector<char>> result = std::make_unique<std::vector<char>>(str.begin(), str.end());
    if (louder)
    {
        result->push_back('!');
        result->push_back('!');
        result->push_back('!');
        result->push_back('!');
    }
    return result;
}

} // namespace detail
