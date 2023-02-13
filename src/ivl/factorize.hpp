#pragma once

#include <vector>
#include <cstdint>
#include <utility>

namespace ivl::nt {

  // this could be a template parameter to everything
  // currently i can't imagine a (reasonable) case where 32bit integer
  // wouldn't be the correct choice
  // if it ever happens, i would recommend wrapping everything
  // in this file in a utility class that would be templated
  // over this
  // (think of it as templated namespace)
  // though that might be annoying since you can't `using namespace <class>;`
  using ExponentType = std::uint32_t;

  // TODO-think: is `std::vector` the best choice for the factorization?
  // no idea, it probably should be customizable, whatever for now
  template<typename T>
  using Factorization = std::vector<std::pair<T, ExponentType>>;

  template<typename T>
  Factorization<T> factorize(T n){
    Factorization<T> factorization;
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

  // for now not mixing factorizations of different types,
  // seems like the safer choice a priori
  template<typename T>
  Factorization<T> merge_factorizations(const Factorization<T>& left, const Factorization<T>& right){
    Factorization<T> out;
    auto it_left = left.begin();
    auto it_right = right.begin();
    while (it_left != left.end() && it_right != right.end()){
      if (it_left->first < it_right->first){
	out.push_back(*it_left);
	++it_left;
      } else if (it_left->first > it_right->first){
	out.push_back(*it_right);
	++it_right;
      } else { // ==
	out.emplace_back({it_left->first, it_left->second + it_right->second});
	++it_left;
	++it_right;
      }
    }
    while (it_left != left.end()){
      out.push_back(*it_left);
      ++it_left;
    }
    while (it_right != right.end()){
      out.push_back(*it_right);
      ++it_right;
    }
    return out;
  }

  // // maintains a factorization of the underlying value
  // // provides nice API regarding the factorization
  // template<typename T>
  // class EagerFactored {
  // private:
  //   T value;
  //   Factorization<T> factorization;

  // public:
  //   explicit EagerFactored(T _value) : value(_value), factorization(factorize(_value)){}

  //   Factorization<T>& get_factorization(){return factorization;}
  //   const Factorization<T>& get_factorization() const {return factorization;}
  //   // TODO: rvalue version?
  //   // something like:
  //   // Factorization<T>&& get_factorization() && {return factorization;}
  //   // need to think about this

  //   EagerFactored& operator*=(const EagerFactored& rhs){
  //     this->value *= rhs.value;
  //     this->factorization = merge_factorizations(this->factorization, rhs.factorization);
  //     return *this;
  //   }

  //   // TODO: rvalue stuff, want to move sometimes
  //   friend EagerFactored operator*(const EagerFactored& left, const EagerFactored& right){
  //     EagerFactored out{left};
  //     out *= right;
  //     return out;
  //   }

  //   // here we see a performance deficit
  //   // `x + EagerFactored{15}` will factor 15, despite it being useless
  //   // rvalue overload would be useless, nothing to move
  //   // TODO: we could use gcd to reduce factorization needed? not sure
  //   EagerFactored& operator+=(const EagerFactored& rhs){
  //     this->value += rhs.value;
  //     this->factorization = factorize(this->value);
  //     return *this;
  //   }

  //   friend EagerFactored operator+(const EagerFactored& left, const EagerFactored& right){
  //     // TODO: switch to {left.value + right.value}, no useless std::vector copy
  //     EagerFactored out{left};
  //     out += right;
  //     return out;
  //   }

  //   EagerFactored& operator-=(const EagerFactored& rhs){
  //     this->value -= rhs.value;
  //     this->factorization = factorize(this->value);
  //     return *this;
  //   }

  //   friend EagerFactored operator-(const EagerFactored& left, const EagerFactored& right){
  //     // TODO: switch to {left.value + right.value}, no useless std::vector copy
  //     EagerFactored out{left};
  //     out -= right;
  //     return out;
  //   }

  //   // operator/= and operator/ INTENTIONALLY not implemented
  //   // there are TWO possibilities for it:
  //   // 1) wrapper around T::operator/, re-do the factorization
  //   // 2) assume divisibility, fix-up the factorization (faster)
  //   // use the explicitly named functions TODO: names

    
  // };

  // // if this was in a different namespace it would have
  // // similar behaviour to `std::swap`
  // // perhaps that implies that `using ivl::nt::factorize;`
  // // would be useful in generic code
  // // TODO-think: should it return a const reference instead of value?
  // // might be best to simply have return type `auto`
  // // or `decltype(auto)` ?
  // // choosing const ref for now
  // // TODO: add rvalue version
  // template<typename T>
  // const Factorization<T>& factorize(const EagerFactored<T>& ef){
  //   return ef.get_factorization();
  // }

  // // this should have same functionality as `EagerFactored`
  // // difference being that it only factors the underlying value
  // // as it is needed
  // template<typename T>
  // class LazyFactored {
  //   static_assert(sizeof(T) != sizeof(T), "THIS IS NOT IMPLEMENTED YET; TODO");
  // };

  // // what happens in LazyFactored::operator*
  // // when one arg is factored, other one isn't?
  // // it discards the factorization :(
  // // a bit wasteful
  // // this doesn't do that
  // template<typename T>
  // class LazyPartialFactored {
  //   static_assert(sizeof(T) != sizeof(T), "THIS IS NOT IMPLEMENTED YET; TODO");
  // };
  
} // namespace ivl::nt