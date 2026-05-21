#include "Assertive.h"
#include <iostream>

namespace
{
    thread_local std::string sA, sB, sComp;

    stack_capture_fn g_stack_capture = &no_stack_capture;

    bool is_library_frame(std::string_view fn)
    {
        // Heuristic: drop frames that come from this library so the reported
        // trace starts at user code. Matches the default backend's symbol
        // shapes; user-provided backends can pre-trim themselves if desired.
        return fn.find("impl::")           != std::string_view::npos
            || fn.find("on_assert_failed") != std::string_view::npos;
    }
}

void set_assert_vars(std::string_view a, std::string_view b, std::string_view comp)
{
    sA = a;
    sB = b;
    sComp = comp;
}

void set_assert_vars(std::string_view a)
{
    set_assert_vars(a, std::string(), std::string());
}

std::vector<stack_frame> no_stack_capture()
{
    return {};
}

stack_capture_fn set_stack_capture(stack_capture_fn new_capture)
{
    stack_capture_fn prev = g_stack_capture;
    g_stack_capture = new_capture ? new_capture : &no_stack_capture;
    return prev;
}

void on_assert_failed(char const* expr, const std::source_location location)
{
    std::cout << "assertion '" << expr << "' failed" << std::endl;
    std::cout << "  in " << location.file_name() << ":" << location.line() << " (" << location.function_name() << ")" << std::endl;
    std::cout << "  expansion: " << sA << " " << sComp << " " << sB << std::endl;

    auto frames = g_stack_capture();
    auto first  = frames.begin();
    while (first != frames.end() && is_library_frame(first->function))
        ++first;
    if (first != frames.end())
    {
        std::cout << "  stack:" << std::endl;
        for (auto it = first; it != frames.end(); ++it)
            std::cout << "    " << it->function
                      << " (" << it->file << ":" << it->line << ")" << std::endl;
    }

    sA.clear();
    sB.clear();
    sComp.clear();
    //std::abort();
}

int main()
{
    ASSERT(1 + 2 == 2);
    ASSERT(1 - 1 >= 2);
    ASSERT(1 != 2 - 1);
    ASSERT(1 < 2 - 2);
    ASSERT(!1);
    ASSERT(0);
    ASSERT(0 + 0 && 1 + 1);
    ASSERT(0 + 0 || 0 + 0);
	return 0;
}

