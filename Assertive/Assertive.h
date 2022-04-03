#pragma once

#include <string_view>
#include <string>
#include <source_location>

void set_assert_vars(std::string_view a, std::string_view b, std::string_view comp);
void set_assert_vars(std::string_view a);
void on_assert_failed(char const* expr, const std::source_location = std::source_location::current());


#define ASSERT(expr) ((impl::assert_t{} < expr) ?                                   \
                       void(0) :                                                    \
                       on_assert_failed(#expr))

namespace impl {


// todo: avoid the copy paste with spaceship operator?
// todo: static assert that op exists for better errors
// todo: factor out the to_string with user defined formatting
#define DEFINE_BINARY_OPERATOR(op)                                              \
    template <class B> bool operator op(B&& b) const {                              \
        if (a op b)                                                                 \
            return true;                                                            \
        set_assert_vars(std::to_string(a), std::to_string(b), #op);                 \
        return false;}                                                              \

    template <class A>
    struct check_t
    {
        A a;

        // for operator !
        operator bool() const { 
            if(static_cast<bool>(a))
                return true;
            set_assert_vars(std::to_string(a));
            return false;
        }

        DEFINE_BINARY_OPERATOR(==)
        DEFINE_BINARY_OPERATOR(!=)
        DEFINE_BINARY_OPERATOR(>=)
        DEFINE_BINARY_OPERATOR(<=)
        DEFINE_BINARY_OPERATOR(<)
        DEFINE_BINARY_OPERATOR(>)
        DEFINE_BINARY_OPERATOR(||)
        DEFINE_BINARY_OPERATOR(&&)
    };

    struct assert_t
    {
        // left-to-right associativity is required so that:
        //  assert_t {} a + b = c will be parsed as 
        // (assert_t {} a + b) = c
        template <class A>
        check_t<A> operator<(A&& a)
        {
            // this code prevents copies
            // if a is an lvalue ref, A is also an lvalue ref, e.g. int&
            // if a is an rvalue ref, A is not a ref and a is moved into check_t
            return check_t<A>{std::forward<A>(a)};
        }
    };
} // namespace impl
