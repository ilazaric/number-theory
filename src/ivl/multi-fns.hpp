#pragma once

// multiplicative functions

#include <ivl/factorize.hpp>
#include <ivl/divisors.hpp>

#include <ivl/tester.hpp>

#include <utility>

namespace ivl::nt {

  namespace compiletime {
    template<auto callable>
    constexpr auto multiplicative_completion =
      []<typename T>(const Factorization<T>& factorization){
      T out{1};
      for (auto [p, e] : factorization){
	out *= callable(p, e);
      }
      return out;    
    };    
  } // namespace compiletime

  constexpr auto tau_compiletime = compiletime::multiplicative_completion<[](auto p, auto e){return e+1;}>;

  namespace runtime {
    constexpr auto multiplicative_completion(auto&& callable){
      return [callable = std::forward<decltype(callable)>(callable)]
	<typename T>(const Factorization<T>& factorization){
	T out{1};
	for (auto [p, e] : factorization){
	  out *= callable(p, e);
	}
	return out;
      };
    }
  } // namespace runtime

  constexpr auto tau_runtime = runtime::multiplicative_completion([](auto p, auto e){return e+1;});
  static_assert(test_equality<Factorization<std::uint32_t>>(tau_compiletime, tau_runtime, 100));

  namespace compiletime {
    template<auto left, auto right>
    constexpr auto dirichlet_convolution =
      []<typename T>(const Factorization<T>& factorization){
      Factorization<T> left_div = factorization;
      Factorization<T> right_div = factorization;
      for (auto& [p, e] : left_div){
	e = 0;
      }
      T out{0};
      while (true){
	out += left(left_div) * right(right_div);
	std::size_t i = 0;
	// TODO: this feels like it can be improved
	for (; i < factorization.size(); ++i){
	  if (right_div[i].second == 0){
	    left_div[i].second = 0;
	    right_div[i].second = factorization[i].second;
	  } else {
	    ++left_div[i].second;
	    --right_div[i].second;
	    break;
	  }
	}
	if (i == factorization.size()){
	  break;
	}
      }
      return out;
    };
  } // namespace compiletime

  // TODO: this might be improveable?
  // not trying anything without perf tests
  template<typename T>
  constexpr T pow(T n, std::uint32_t e){
    T out{1};
    while (e){
      if (e%2 == 1){
	out *= n;
      }
      e /= 2;
      n *= n;
    }
    return out;
  }

  // this is 1 + n + n^2 + ... + n^(e-1)
  // equal to (n^e - 1) / (n - 1)
  // but not implemented directly via that
  // bc i am afraid of overflow
  // for example: powsum(1000001, 1)
  template<typename T>
  constexpr T powsum(T n, std::uint32_t e){
    if (e == 0) return 1;
    T prev = ::ivl::nt::pow(n, e-1);
    // TODO: the inner-most -1 is kinda not needed?
    // should i remove it?
    return (prev-1) / (n-1) + prev;
  }

  constexpr auto one = [](const auto&){return 1;};
  // great reason why factorization should be lazy
  // or `Factorization<T>` should contain the arg as well
  // 2nd is easier to do
  constexpr auto id = []<typename T>(const Factorization<T> factorization){
    T out{1};
    for (auto [p, e] : factorization){
      out *= ::ivl::nt::pow(p, e);
    }
    return out;
  };

  constexpr auto tau_dirichlet = compiletime::dirichlet_convolution<one, one>;
  static_assert(test_equality<Factorization<std::uint32_t>>(tau_compiletime, tau_dirichlet, 100));

  constexpr auto sigma_dirichlet = compiletime::dirichlet_convolution<one, id>;
  
  // constexpr auto tau_v4 = multiplicative_completion3<[](auto p, auto e){return e+1;}>;

  // constexpr auto dirichlet_convolution(auto&& left, auto&& right){
  //   return [left = std::forward<decltype(left)>(left),
  // 	    right = std::forward<decltype(right)>(right)
  // 	    ]<typename T, typename tag_type = regular_number_tag>(T&& arg, tag_type = regular_number_tag{}){
  //     if constexpr (std::is_same_v<regular_number_tag, std::remove_cvref_t<tag_type>>){
  // 	auto factorization = factorize(arg);
  // 	std::remove_cvref_t<decltype(factorization.front().first)> out{0};
  // 	auto divisors = generate_all_divisors(factorization);
  // 	for (uint32_t i = 0; i < divisors.size(); ++i){
  // 	  const auto a = divisors[i];
  // 	  const auto b = divisors[divisors.size()-1-i];
  // 	  out += left(a) * right(b);
  // 	}
  // 	return out;
  //     } else {
  // 	std::remove_cvref_t<decltype(arg.front().first)> out{0};
  // 	auto divisors = generate_all_divisors(arg);
  // 	for (uint32_t i = 0; i < divisors.size(); ++i){
  // 	  const auto a = divisors[i];
  // 	  const auto b = divisors[divisors.size()-1-i];
  // 	  out += left(a) * right(b);
  // 	}
  // 	return out;
  //     }
  //   };
  // }

  // constexpr auto one = multiplicative_completion([](auto p, auto e){return 1;});
  // constexpr auto tau_v2 = dirichlet_convolution(one, one);
  
} // namespace ivl::nt
