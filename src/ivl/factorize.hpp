#pragma once

#include <cstdint>
#include <utility>
#include <vector>

namespace ivl::nt {

// this comment is becoming more and more outdated
//
// this could be a template parameter to everything
// currently i can't imagine a (reasonable) case where 32bit integer
// wouldn't be the correct choice
// if it ever happens, i would recommend wrapping everything
// in this file in a utility class that would be templated
// over this
// (think of it as templated namespace)
// though that might be annoying since you can't `using namespace <class>;`
using ExponentType = std::uint32_t;

// TODO-think: is `std::vector` the best choice for the factorization?
// no idea, it probably should be customizable, whatever for now
// TODO: make this a wrapper around `std::vector`, this is prone to bugs:
// Factorization<int> f{100}; <-- doesn't do what a lot of people would think
template<typename T, typename ET = ExponentType> using Factorization = std::vector<std::pair<T, ET>>;

class ZeroFactorizationException : public std::exception
{
public:
  virtual const char *what() const noexcept override { return "tried to factorize 0 (zero)"; }
};

template<typename T, typename ET = ExponentType> constexpr Factorization<T, ET> factorize(T n)
{
  if (n < T{ 0 }) n = -n;
  // this could be moved into a contract if those existed in C++
  if (n == T{ 0 }) throw ZeroFactorizationException{};
  Factorization<T, ET> factorization;
  for (T p{ 2 }; p * p <= n; ++p) {
    if (n % p == T{ 0 }) {
      factorization.emplace_back(p, 0);
      while (n % p == T{ 0 }) {
        n /= p;
        ++factorization.back().second;
      }
    }
  }
  if (n != T{ 1 }) { factorization.emplace_back(n, 1); }
  return factorization;
}

template<typename T, typename ET = ExponentType>
constexpr const Factorization<T, ET> &factorize(const Factorization<T, ET> &f)
{
  return f;
}

static_assert(factorize<int, std::uint32_t>(2100)
              == std::vector<std::pair<int, std::uint32_t>>{ { 2, 2 }, { 3, 1 }, { 5, 2 }, { 7, 1 } });

// for now not mixing factorizations of different types,
// seems like the safer choice a priori
template<typename T>
constexpr Factorization<T> merge_factorizations(const Factorization<T> &left, const Factorization<T> &right)
{
  Factorization<T> out;
  auto it_left = left.begin();
  auto it_right = right.begin();
  while (it_left != left.end() && it_right != right.end()) {
    if (it_left->first < it_right->first) {
      out.push_back(*it_left);
      ++it_left;
    } else if (it_left->first > it_right->first) {
      out.push_back(*it_right);
      ++it_right;
    } else {// ==
      out.emplace_back({ it_left->first, it_left->second + it_right->second });
      ++it_left;
      ++it_right;
    }
  }
  while (it_left != left.end()) {
    out.push_back(*it_left);
    ++it_left;
  }
  while (it_right != right.end()) {
    out.push_back(*it_right);
    ++it_right;
  }
  return out;
}


}// namespace ivl::nt
