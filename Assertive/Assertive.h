#pragma once

#include <string_view>
#include <format>
#include <source_location>

void set_assert_vars(std::string_view a, std::string_view b, std::string_view comp);
void set_assert_vars(std::string_view a);
void on_assert_failed(char const* expr, const std::source_location = std::source_location::current());

// std::formattable is C++23 only; this is the C++20-portable equivalent.
template <class T>
concept formattable_for_assert = requires(T const& v) {
    std::format("{}", v);
};


#define ASSERT(expr) ((impl::assert_t{} < expr) ?                                   \
                       void(0) :                                                    \
                       on_assert_failed(#expr))

namespace impl {


// todo: avoid the copy paste with spaceship operator?
// todo: static assert that op exists for better errors
#define DEFINE_BINARY_OPERATOR(op)                                              \
    template <class B> bool operator op(B&& b) const {                              \
        if (a op b)                                                                 \
            return true;                                                            \
        static_assert(formattable_for_assert<A>,                                    \
            "ASSERT: left operand type has no std::formatter<T> specialization. "   \
            "Specialize std::formatter for your type.");                            \
        static_assert(formattable_for_assert<B>,                                    \
            "ASSERT: right operand type has no std::formatter<T> specialization. " \
            "Specialize std::formatter for your type.");                            \
        set_assert_vars(std::format("{}", a), std::format("{}", b), #op);           \
        return false;}                                                              \

    template <class A>
    struct check_t
    {
        A a;

        // for operator !
        operator bool() const {
            if(static_cast<bool>(a))
                return true;
            static_assert(formattable_for_assert<A>,
                "ASSERT: operand type has no std::formatter<T> specialization. "
                "Specialize std::formatter for your type.");
            set_assert_vars(std::format("{}", a));
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
