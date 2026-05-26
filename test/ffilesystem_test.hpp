#include <string>
#include <string_view>
#include <tuple>

#include <boost/ut.hpp>

// based on template from example/matcher.cpp of Boost.UT
// https://github.com/boost-ext/ut/blob/master/example/matcher.cpp
// but modified considerably to duck-type string-like types

template <class... Ts>
class any_of {
public:
    constexpr explicit any_of(Ts... ts) : ts_{ts...} {}

    constexpr auto operator==(std::string_view t) const {
        return std::apply([t](const auto&... args) {
            return eq(t, to_sv(args)...);
        }, ts_);
    }

private:
    static constexpr std::string_view to_sv(std::string_view value) {
        return value;
    }

    static std::string_view to_sv(const std::string& value) {
        return value;
    }

    static std::string_view to_sv(const char* value) {
        return value ? std::string_view{value} : std::string_view{};
    }

    template <class T, class U, class... TArgs>
    static constexpr auto eq(const T& t, const U& u, const TArgs&... args) {
        using namespace boost::ut;
        if constexpr (sizeof...(args) > 0) {
            return (that % detail::value{u} == t) or eq(t, args...);
        } else {
            return (that % detail::value{u} == t);
        }
    }

    std::tuple<Ts...> ts_;
};
