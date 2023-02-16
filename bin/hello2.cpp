#include <ivl/divisors.hpp>
#include <ivl/factorize.hpp>
#include <ivl/lazy.hpp>
#include <ivl/multi-fns.hpp>

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

  std::uint32_t n;
  while (std::cin >> n) {
    ivl::nt::Lazy lazy{ n };

    std::cout << "fact(" << n << ") -- ";
    for (auto [p, e] : factorize(lazy)) std::cout << p << "^" << e << " ";
    std::cout << std::endl;

    ivl::nt::DivisorIterable di{ factorize(lazy) };
    std::cout << "divisors: ";
    for (auto d : ivl::nt::DivisorIterable{ factorize(lazy) }) std::cout << ivl::nt::id(d) << " ";
    // std::cout << "[" << d << "] ";
    std::cout << std::endl;

    std::cout << "tau(" << n << ") -- " << ivl::nt::tau_compiletime(factorize(lazy)) << std::endl;
    std::cout << "tau(" << n << ") -- " << ivl::nt::tau_runtime(factorize(lazy)) << std::endl;
    std::cout << "tau(" << n << ") -- " << ivl::nt::tau_dirichlet(factorize(lazy)) << std::endl;
  }

  return 0;
}
