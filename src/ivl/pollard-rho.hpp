#pragma once

#include <algorithm>

namespace ivl::nt::pollard_rho {

template<typename T> T gcd(T a, T b)
{
  using std::swap;
  while (b) {
    a %= b;
    swap(a, b);
  }
  return a;
}

template<typename T, auto Poly = [](T x) -> T { return x * x + 1; }> T magic_algo(T n, T x)
{
  T y{ x };
  T d{ 1 };
  while (d == T{ 1 }) {
    x = Poly(x) % n;
    y = Poly(Poly(y) % n) % n;
    d = gcd(x < y ? y - x : x - y, n);
  }
  return d;
}

}// namespace ivl::nt::pollard_rho
