#include <random>
#include <algorithm>            // for generate_n
#include <array>                // for array
#include <functional>           // for ref
#include <iterator>             // for begin, end

#include <string>
#include <string_view>

#include "ffilesystem.h"


// CTAD C++17 random string generator
// https://stackoverflow.com/a/444614
// https://en.cppreference.com/w/cpp/language/class_template_argument_deduction

#ifdef __cpp_deduction_guides  // C++17
template <typename T = std::mt19937>

static auto fs_random_generator() -> T {
  auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
  auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
  auto seed = std::array<std::seed_seq::result_type, seed_len>();
  auto dev = std::random_device();
  std::generate_n(std::begin(seed), seed_len, std::ref(dev));
  auto seed_seq = std::seed_seq(std::begin(seed), std::end(seed));
  return T{seed_seq};
}
#endif

std::string fs_generate_random_alphanumeric_string(const std::string::size_type len)
{
#ifdef __cpp_deduction_guides
  constexpr std::string_view chars =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  thread_local auto rng = fs_random_generator<>();
  auto dist = std::uniform_int_distribution<std::string::size_type>(0, chars.length() - 1);
  auto result = std::string(len, '\0');
  std::generate_n(std::begin(result), len, [&]() { return chars[dist(rng)]; });
  return result;
#else
  fs_print_error("" , __func__, std::make_error_code(std::errc::function_not_supported));
  return {};
#endif
}
