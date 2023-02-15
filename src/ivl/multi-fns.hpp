#pragma once

// multiplicative functions

#include <ivl/divisors.hpp>
#include <ivl/factorize.hpp>

#include <ivl/tester.hpp>

#include <utility>

namespace ivl::nt {

namespace compiletime {
template <auto callable>
constexpr auto multiplicative_completion =
    []<typename T>(const Factorization<T> &factorization) {
      T out{1};
      for (auto [p, e] : factorization) {
        out *= callable(p, e);
      }
      return out;
    };
} // namespace compiletime

constexpr auto tau_compiletime =
    compiletime::multiplicative_completion<[](auto p, auto e) {
      return e + 1;
    }>;

namespace runtime {
constexpr auto multiplicative_completion(auto &&callable) {
  return [callable = std::forward<decltype(callable)>(callable)]<typename T>(
             const Factorization<T> &factorization) {
    T out{1};
    for (auto [p, e] : factorization) {
      out *= callable(p, e);
    }
    return out;
  };
}
} // namespace runtime

constexpr auto tau_runtime =
    runtime::multiplicative_completion([](auto p, auto e) { return e + 1; });
static_assert(test_equality<Factorization<std::uint32_t>>(tau_compiletime,
                                                          tau_runtime, 100));

namespace compiletime {
template <auto left, auto right>
constexpr auto dirichlet_convolution =
    []<typename T>(const Factorization<T> &factorization) {
      Factorization<T> left_div = factorization;
      Factorization<T> right_div = factorization;
      for (auto &[p, e] : left_div) {
        e = 0;
      }
      T out{0};
      while (true) {
        out += left(left_div) * right(right_div);
        std::size_t i = 0;
        // TODO: this feels like it can be improved
        for (; i < factorization.size(); ++i) {
          if (right_div[i].second == 0) {
            left_div[i].second = 0;
            right_div[i].second = factorization[i].second;
          } else {
            ++left_div[i].second;
            --right_div[i].second;
            break;
          }
        }
        if (i == factorization.size()) {
          break;
        }
      }
      return out;
    };
} // namespace compiletime

// implemented as a lambda so i can manipulate the object
// `pow.operator()` is probably equivalent to
// function template implementation
// TODO: this might be improveable?
// not trying anything without perf tests
constexpr auto pow = []<typename T>(T n, std::uint32_t e) -> T {
  T out{1};
  while (e) {
    if (e % 2 == 1) {
      out *= n;
    }
    e /= 2;
    n *= n;
  }
  return out;
};

// this is 1 + n + n^2 + ... + n^(e-1)
// equal to (n^e - 1) / (n - 1)
// but not implemented directly via that
// bc i am afraid of overflow
// for example: powsum(1000001, 1)
// implemented as a lambda so i can manipulate the object
// `powsum.operator()` is probably equivalent to
// function template implementation
constexpr auto powsum = []<typename T>(T n, std::uint32_t e) -> T {
  if (e == 0)
    return 1;
  if (n == 1)
    return e + 1;
  T prev = ::ivl::nt::pow(n, e - 1);
  // TODO: the inner-most -1 is kinda not needed?
  // should i remove it?
  return (prev - 1) / (n - 1) + prev;
};

constexpr auto sigma_compiletime =
    compiletime::multiplicative_completion<powsum>;

// TODO-think: should it be auto&& ?
// the compiler is ~probably~ smart enough to not copy
constexpr auto one = [](auto) { return 1; };
// great reason why factorization should be lazy
// or `Factorization<T>` should contain the arg as well
// 2nd is easier to do
// 2nd actually is implied by 1st
constexpr auto id = []<typename T>(const Factorization<T> factorization) {
  T out{1};
  for (auto [p, e] : factorization) {
    out *= ::ivl::nt::pow(p, e);
  }
  return out;
};

// this is the neutral element with respect to the dirichlet convolution
constexpr auto epsilon = []<typename T>(const Factorization<T> factorization) {
  return id(factorization) == 1 ? 1 : 0;
};
static_assert(test_equality<Factorization<std::uint32_t>>(
    sigma_compiletime,
    compiletime::dirichlet_convolution<sigma_compiletime, epsilon>, 100));

constexpr auto tau_dirichlet = compiletime::dirichlet_convolution<one, one>;
static_assert(test_equality<Factorization<std::uint32_t>>(tau_compiletime,
                                                          tau_dirichlet, 100));

constexpr auto sigma_dirichlet = compiletime::dirichlet_convolution<one, id>;
static_assert(test_equality<Factorization<std::uint32_t>>(sigma_compiletime,
                                                          sigma_dirichlet,
                                                          100));

constexpr auto id_compiletime = compiletime::multiplicative_completion<pow>;
static_assert(test_equality<Factorization<std::uint32_t>>(id_compiletime, id,
                                                          100));

} // namespace ivl::nt
