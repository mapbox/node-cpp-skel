#pragma once

#include <chrono>
#include <string>
#include <thread>

namespace detail {

// simulate CPU intensive task
inline std::unique_ptr<std::string> do_expensive_work(std::string const& name, bool louder)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::unique_ptr<std::string> result = std::make_unique<std::string>("...threads are busy async bees...hello " + name);
    if (louder)
    {
        *result += "!!!!";
    }
    return result;
}

} // namespace detail
