#include <ivl/bignum.hpp>
#include <ivl/divisors.hpp>
#include <ivl/factorize.hpp>
#include <ivl/lazy.hpp>
#include <ivl/multi-fns.hpp>
#include <ivl/pollard-rho.hpp>
#include <ivl/safe.hpp>

#include <iostream>

template<typename T, typename U> std::ostream &operator<<(std::ostream &out, const std::pair<T, U> &p)
{
  return out << p.first << "^" << p.second;
}

template<typename T> std::ostream &operator<<(std::ostream &out, const std::vector<T> &vec)
{
  for (const auto &x : vec) out << x << " ";
  return out;
}

int main()
{
  using ::ivl::nt::factorize;

  // using Bignum = ivl::nt::UnsignedBignum<std::uint32_t, 10>;
  using Bignum = ivl::nt::Bignum<std::uint32_t, 10>;

  // static_assert(std::three_way_comparable<Bignum>);

  std::int64_t a, b;
  while (std::cin >> a >> b) {
    Bignum x{ a }, y{ b };
    std::cout << x << " + " << y << " == " << x + y << std::endl;
    std::cout << x << " - " << y << " == " << x - y << std::endl;
    std::cout << x << " * " << y << " == " << x * y << std::endl;
    std::cout << x << " / " << y << " == " << x / y << std::endl;
    std::cout << x << " / " << y << " == " << x % y << std::endl;
    // std::cout << x << " < " << y << " == " << (x<y) << std::endl;
    // std::cout << x << " > " << y << " == " << (x>y) << std::endl;
    // std::cout << x << " <= " << y << " == " << (x<=y) << std::endl;
    // std::cout << x << " >= " << y << " == " << (x>=y) << std::endl;
    // std::cout << x << " == " << y << " == " << (x==y) << std::endl;
    // std::cout << x << " != " << y << " == " << (x!=y) << std::endl;
  }

  return 0;
}
