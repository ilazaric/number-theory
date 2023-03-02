// #include <ivl/bignum.hpp>
#include <flint/fmpzxx.h>

#include <iostream>

int main()
{
  using Integer = flint::fmpzxx;

  Integer a{ 123 };
  Integer b{ 10219 };
  Integer c{ 1092 };

  for (std::uint32_t i = 0; i < 5000; ++i) { c = c * a + a * c + b; }

  c %= Integer{ 1000000007 };
  std::cout << c << std::endl;

  return 0;
}
