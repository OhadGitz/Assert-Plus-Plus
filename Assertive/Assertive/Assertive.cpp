#include "Assertive.h"
#include <iostream>

namespace
{
    thread_local std::string sA, sB, sComp;
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

void on_assert_failed(char const* expr, const std::source_location location)
{
    std::cout << "assertion '" << expr << "' failed" << std::endl;
    std::cout << "  in " << location.file_name() << ":" << location.line() << " (" << location.function_name() << ")" << std::endl;
    std::cout << "  expansion: " << sA << " " << sComp << " " << sB << std::endl;
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

