#pragma once

namespace ivl::nt {

// very sexy for static_assert tests
  template <typename T = std::uint32_t>
constexpr bool test_equality(auto &&left, auto &&right, std::uint32_t limit) {
  for (std::uint32_t i = 1; i <= limit; ++i) {
    T arg{i};
    if (left(arg) != right(arg)) {
      return false;
    }
  }
  return true;
}

} // namespace ivl::nt
