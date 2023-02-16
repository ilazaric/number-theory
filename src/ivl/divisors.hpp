#pragma once

#include <ivl/factorize.hpp>

#include <functional>

namespace ivl::nt {

// fun fact: first * last = 2nd * 2nd last = ...
template<typename T> std::vector<T> generate_all_divisors(const Factorization<T> &factorization)
{
  std::uint32_t full_count = 1;
  for (auto [p, e] : factorization) { full_count *= e + 1; }// full_count = tau
  std::vector<T> out(full_count);
  std::uint32_t count = 1;
  out[0] = 1;
  for (auto [p, e] : factorization) {
    const auto prev_count = count;
    for (uint32_t i = 0; i < e * prev_count; ++i) {
      out[count] = out[count - prev_count] * p;
      ++count;
    }
  }
  return out;
}

template<typename T> class DivisorIterable
{
private:
  std::reference_wrapper<const Factorization<T>> factorization;

public:
  DivisorIterable(const Factorization<T> &_factorization) : factorization(_factorization) {}

  class Iterator;
  friend class Iterator;

  Iterator begin() const { return Iterator{ *this }; }
  Iterator end() const { return Iterator::end; }

  // TODO: iterator_tag stuff
  class Iterator
  {
  private:
    // not reference wrapper bc we nullify it in end
    const Factorization<T> *factorization;
    // TODO-think: oof this makes me feel like
    // the correct choice was [[p1, p2, ...], [e1, e2, ...]]
    // instead of [[p1, e2], [p2, e2], ...]
    Factorization<T> divisor;

    // to construct `end` singleton
    Iterator() : factorization(nullptr), divisor() {}

  public:
    inline static const Iterator end;

    explicit Iterator(const DivisorIterable &parent)
      : factorization(&(parent.factorization.get())), divisor(*factorization)
    {
      for (auto &[p, e] : divisor) e = 0;
    }

    Iterator &operator++()
    {
      std::size_t i = 0;
      // TODO: refactor
      for (; i < divisor.size(); ++i) {
        if (++divisor[i].second == (*factorization)[i].second + 1) {
          divisor[i].second = 0;
        } else {
          break;
        }
      }
      if (i == divisor.size()) {
        // end
        factorization = nullptr;
        divisor.clear();
      }
      return *this;
    }

    const Factorization<T> &operator*() const { return divisor; }

    friend bool operator==(const Iterator &left, const Iterator &right)
    {
      return left.factorization == right.factorization && left.divisor == right.divisor;
    }
  };
};

}// namespace ivl::nt
