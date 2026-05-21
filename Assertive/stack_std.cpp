#include "Assertive.h"

#include <cstddef>
#include <stacktrace>
#include <string>
#include <vector>

namespace
{
    // Backing storage for the string_view members of the returned stack_frames.
    // thread_local: capture runs on the asserting thread; views stay valid
    // until the next capture call on this thread (matches the contract
    // documented in Assertive.h).
    thread_local std::vector<std::string> g_storage;

    std::vector<stack_frame> std_stack_capture()
    {
        auto raw = std::stacktrace::current();

        g_storage.clear();
        g_storage.reserve(raw.size() * 2);
        for (auto const& entry : raw)
        {
            g_storage.push_back(entry.description());
            g_storage.push_back(entry.source_file());
        }

        std::vector<stack_frame> frames;
        frames.reserve(raw.size());
        std::size_t idx = 0;
        for (auto const& entry : raw)
        {
            frames.push_back(stack_frame{
                g_storage[idx],
                g_storage[idx + 1],
                static_cast<int>(entry.source_line())
            });
            idx += 2;
        }
        return frames;
    }

    struct installer
    {
        installer() { set_stack_capture(&std_stack_capture); }
    };
    installer s_installer;
}
