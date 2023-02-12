#pragma once

#include <vector>
#include <cstdint>

namespace ivl::nt {

  template<typename T, typename E = std::uint32_t>
  std::vector<std::pair<T, E>> factorize(T n){
    std::vector<std::pair<T, E>> factorization;
    for (T p = 2; p*p <= n; ++p){
      if (n%p == 0){
        factorization.emplace_back(p, 0);
	while (n%p == 0){
	  n /= p;
	  ++factorization.back().second;
	}
      }
    }
    if (n != 1){
      factorization.emplace_back(n, 1);
    }
    return factorization;
  }
  
} // namespace ivl::nt
