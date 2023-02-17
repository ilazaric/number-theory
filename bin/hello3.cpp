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

  std::int64_t n;
  while (std::cin >> n) { std::cout << ivl::nt::pollard_rho::magic_algo<std::int64_t>(n, 2) << std::endl; }

  return 0;
}
