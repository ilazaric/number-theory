#include <ivl/bignum.hpp>

#include <iostream>

int main()
{
  using Integer = ivl::nt::Bignum<std::uint32_t, 10>;

  Integer a{ 123 };
  Integer b{ 10219 };
  Integer c{ 1092 };

  for (std::uint32_t i = 0; i < 5000; ++i) { c = c * a + a * c + b; }

  c %= Integer{ 1000000007 };
  std::cout << c.template get<std::int64_t>() << std::endl;

  return 0;
}
