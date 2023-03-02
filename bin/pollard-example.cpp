// #include <ivl/bignum.hpp>
// #include <ivl/pollard-rho.hpp>
// #include <gmpxx.h>
#include <flint/fmpz_factorxx.h>
#include <flint/fmpzxx.h>

#include <iostream>
#include <string>

// using Integer = ivl::nt::Bignum<std::uint64_t, (1ULL << 31)>;
using Integer = flint::fmpzxx;

int main()
{

  std::string input;
  while (std::cin >> input) {
    Integer parsed;
    for (auto c : input) parsed = parsed * Integer{ 10 } + Integer{ c - '0' };
    std::cout << "input: " << parsed << std::endl;
    // std::cout << "magic: " << ivl::nt::pollard_rho::magic_algo(parsed, Integer{ 2 }) << std::endl;
    auto f = flint::factor(parsed);
    // f.print();
    for (ssize_t i = 0; i < static_cast<ssize_t>(f.size()); ++i) std::cout << f.p(i) << "^" << f.exp(i) << " ";
    std::cout << std::endl;
  }

  return 0;
}
