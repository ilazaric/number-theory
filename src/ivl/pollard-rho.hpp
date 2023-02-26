#pragma once

#include <algorithm>
#include <iostream>

namespace ivl::nt::pollard_rho {

template<typename T> T gcd(T a, T b)
{
  using std::swap;
  while (b) {
    a %= b;
    // if (a + a > b) a = b - a;
    swap(a, b);
  }
  return a;
}

template<typename T, auto Poly = [](T x) -> T { return x * x + 1; }> T magic_algo(T n, T x)
{
  T y{ x };
  T d{ 1 };
  std::uint64_t count = 0;
  while (d == T{ 1 }) {
    ++count;
    // std::cout << count << std::endl;
    if (count % 10000 == 0) {
      std::cout << "." << std::flush;
      // std::cout << x << std::endl << y << std::endl << d << std::endl << std::endl;
    }
    x = Poly(x) % n;
    y = Poly(Poly(y) % n) % n;
    d = gcd(x < y ? y - x : x - y, n);
  }
  return d;
}

}// namespace ivl::nt::pollard_rho
