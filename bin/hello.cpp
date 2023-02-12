#include <ivl/factorize.hpp>

#include <iostream>

int main(){
  std::uint32_t n;
  while (std::cin >> n){
    auto f = ivl::nt::factorize(n);
    std::cout << n << " -- ";
    for (auto [p, e] : f)
      std::cout << p << "^" << e << " ";
    std::cout << std::endl;
  }
  return 0;
}
