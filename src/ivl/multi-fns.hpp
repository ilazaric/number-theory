#pragma once

// multiplicative functions

#include <ivl/factorize.hpp>
#include <ivl/divisors.hpp>

#include <utility>

namespace ivl::nt {

  struct regular_number_tag {};
  struct factorization_tag {};

  constexpr auto multiplicative_completion(auto&& callable){
    return [callable = std::forward<decltype(callable)>(callable)]<typename T, typename tag_type = regular_number_tag>(T&& arg, tag_type = regular_number_tag{}){
      if constexpr (std::is_same_v<regular_number_tag, std::remove_cvref_t<tag_type>>){
	auto factorization = factorize(arg);
	std::remove_cvref_t<T> out{1};
	for (auto [p, e] : factorization){
	  out *= callable(p, e);
	}
	return out;
      } else {
	std::remove_cvref_t<decltype(arg.front().first)> out{1};
	for (auto [p, e] : arg){
	  out *= callable(p, e);
	}
	return out;
      }
    };
  }

  constexpr auto tau_v1 = multiplicative_completion([](auto p, auto e){return e + 1;});

  constexpr auto dirichlet_convolution(auto&& left, auto&& right){
    return [left = std::forward<decltype(left)>(left),
	    right = std::forward<decltype(right)>(right)
	    ]<typename T, typename tag_type = regular_number_tag>(T&& arg, tag_type = regular_number_tag{}){
      if constexpr (std::is_same_v<regular_number_tag, std::remove_cvref_t<tag_type>>){
	auto factorization = factorize(arg);
	std::remove_cvref_t<decltype(factorization.front().first)> out{0};
	auto divisors = generate_all_divisors(factorization);
	for (uint32_t i = 0; i < divisors.size(); ++i){
	  const auto a = divisors[i];
	  const auto b = divisors[divisors.size()-1-i];
	  out += left(a) * right(b);
	}
	return out;
      } else {
	std::remove_cvref_t<decltype(arg.front().first)> out{0};
	auto divisors = generate_all_divisors(arg);
	for (uint32_t i = 0; i < divisors.size(); ++i){
	  const auto a = divisors[i];
	  const auto b = divisors[divisors.size()-1-i];
	  out += left(a) * right(b);
	}
	return out;
      }
    };
  }

  constexpr auto one = multiplicative_completion([](auto p, auto e){return 1;});
  constexpr auto tau_v2 = dirichlet_convolution(one, one);

  // template<auto Callable1,
  // 	   auto Callable2,
  // 	   typename T,
  // 	   typename E = std::uint32_t>
  // T dirichlet_convolution_f(const Factorization<T>& factorization){
    
  // }
  
} // namespace ivl::nt
