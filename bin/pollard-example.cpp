#include <ivl/bignum.hpp>
#include <ivl/pollard-rho.hpp>

#include <iostream>
#include <string>

using Integer = ivl::nt::Bignum<std::uint64_t, (1ULL << 31)>;

int main()
{

  std::string input;
  while (std::cin >> input) {
    Integer parsed;
    for (auto c : input) parsed = parsed * Integer{ 10 } + Integer{ c - '0' };
    std::cout << "input: " << parsed << std::endl;
    std::cout << "magic: " << ivl::nt::pollard_rho::magic_algo(parsed, Integer{ 2 }) << std::endl;
  }

  return 0;
}
