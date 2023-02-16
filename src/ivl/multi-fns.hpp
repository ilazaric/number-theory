#pragma once

// multiplicative functions

#include <ivl/divisors.hpp>
#include <ivl/factorize.hpp>

#include <ivl/tester.hpp>

#include <utility>

namespace ivl::nt {

  // multiplicative functions with argument of type T
  // return this type
  // probably not perfect for every case but should be okay in general
  template<typename T>
  using ReturnType = std::remove_cvref_t<decltype(factorize(std::declval<T>()).front().first)>;

namespace compiletime {
template <auto callable>
constexpr auto multiplicative_completion =
    []<typename T>(T&& arg) -> ReturnType<T> {
  // TODO: check if correct
      decltype(auto) factorization = factorize(arg);
      ReturnType<T> out{1};
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
             T&& arg) -> ReturnType<T> {
    decltype(auto) factorization = factorize(arg);
    ReturnType<T> out{1};
    for (auto [p, e] : factorization) {
      out *= callable(p, e);
    }
    return out;
  };
}
} // namespace runtime

constexpr auto tau_runtime =
    runtime::multiplicative_completion([](auto p, auto e) { return e + 1; });
static_assert(test_equality(tau_compiletime,
                                                          tau_runtime, 100));

namespace compiletime {
template <auto left, auto right>
constexpr auto dirichlet_convolution =
    []<typename T>(T&& arg) -> ReturnType<T> {
      decltype(auto) factorization = factorize(arg);
      auto left_div = factorization;
      auto right_div = factorization;
      for (auto &[p, e] : left_div) {
        e = 0;
      }
      ReturnType<T> out{0};
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

// this is 1 + n + n^2 + ... + n^e
// equal to (n^(e+1) - 1) / (n - 1)
// but not implemented directly via that
// bc i am afraid of overflow
// for example: powsum(1000001, 1)
// implemented as a lambda so i can manipulate the object
// `powsum.operator()` is probably equivalent to
// function template implementation
constexpr auto powsum = []<typename T>(T n, std::uint32_t e) -> T {
  if (n == 1)
    return e + 1;
  T tmp = ::ivl::nt::pow(n, e);
  // TODO: the inner-most -1 is kinda not needed?
  // should i remove it?
  return (tmp - 1) / (n - 1) + tmp;
};

constexpr auto sigma_compiletime =
    compiletime::multiplicative_completion<powsum>;

// TODO-think: should it be T&& ?
// the compiler is ~probably~ smart enough to not copy
  constexpr auto one = []<typename T>(T) -> ReturnType<T> { return 1; };
constexpr auto id = []<typename T>(T&& arg) -> ReturnType<T> {
  if constexpr (std::is_same_v<ReturnType<T>, std::remove_cvref_t<T>>){
    return arg;
  } else {
    ReturnType<T> out{1};
    for (auto [p, e] : arg) {
      out *= ::ivl::nt::pow(p, e);
    }
    return out;
  }
};

// this is the neutral element with respect to the dirichlet convolution
constexpr auto epsilon = []<typename T>(T&& arg) -> ReturnType<T> {
  return id(arg) == 1 ? 1 : 0;
};
static_assert(test_equality(
    sigma_compiletime,
    compiletime::dirichlet_convolution<sigma_compiletime, epsilon>, 100));

constexpr auto tau_dirichlet = compiletime::dirichlet_convolution<one, one>;
static_assert(test_equality(tau_compiletime,
                                                          tau_dirichlet, 100));

constexpr auto sigma_dirichlet = compiletime::dirichlet_convolution<one, id>;
static_assert(test_equality(sigma_compiletime,
                                                          sigma_dirichlet,
                                                          100));

constexpr auto id_compiletime = compiletime::multiplicative_completion<pow>;
static_assert(test_equality(id_compiletime, id,
                                                          100));

} // namespace ivl::nt
