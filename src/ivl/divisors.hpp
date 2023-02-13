#pragma once

#include <ivl/factorize.hpp>

#include <functional>

namespace ivl::nt {

  template<typename T>
  std::vector<T> generate_all_divisors(const Factorization<T>& factorization){
    std::uint32_t full_count = 1;
    for (auto [p, e] : factorization){
      full_count *= e+1;
    } // full_count = tau
    std::vector<T> out(full_count);
    std::uint32_t count = 1;
    out[0] = 1;
    for (auto [p, e] : factorization){
      const auto prev_count = count;
      for (uint32_t i = 0; i < e*prev_count; ++i){
	out[count] = out[count-prev_count] * p;
	++count;
      }
    }
    return out;
  }

  template<typename T>
  class DivisorIterable {
  private:
    std::reference_wrapper<const Factorization<T>> factorization;
    
  public:
    DivisorIterable(const Factorization<T>& _factorization) :
      factorization(_factorization){}

    class Iterator {
    private:
      std::reference_wrapper<const Factorization<T>> factorization;
      std::vector<ExponentType> divisor_factorization; // size == factorization.size + 1
      T divisor;
    };
  };

  // template<typename T>
  // class DivisorSymmetricIterable {
  // };

} // namespace ivl::nt
