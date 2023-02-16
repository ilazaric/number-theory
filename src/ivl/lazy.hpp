#pragma once

#include <ivl/factorize.hpp>

#include <optional>
#include <utility>
#include <vector>

namespace ivl::nt {

// TODO: custom factorize strategy
struct LazyDefaultTraits {
  using ValueType = std::int64_t;
  using ExponentType = std::uint32_t;
};

template <typename Traits = LazyDefaultTraits> class Lazy {
private:
  using ValueType = Traits::ValueType;
  using ExponentType = Traits::ExponentType;
  using FactorizationType = std::vector<std::pair<ValueType, ExponentType>>;

  ValueType m_value;
  mutable std::optional<FactorizationType> m_factorization;

public:
  explicit constexpr Lazy(ValueType value)
      : m_value(value), m_factorization(std::nullopt) {}

  const FactorizationType &get_factorization() const {
    if (m_factorization)
      return *m_factorization;
    // not needed atm, might be needed if i do some crazy refactoring
    using ::ivl::nt::factorize;
    m_factorization.emplace(factorize(m_value));
    return *m_factorization;
  }

  Lazy &operator*=(const Lazy &arg) {
    if (this->m_factorization && arg.m_factorization) {
      this->m_factorization =
          merge_factorizations(this->m_factorization, arg.m_factorization);
    } else {
      this->m_factorization = std::nullopt;
    }
    this->m_value *= arg.m_value;
    return *this;
  }

  // TODO: this copies a vector for potentially no reason
  friend Lazy operator*(const Lazy &left, const Lazy &right) {
    Lazy out{left};
    out *= right;
    return out;
  }

  // no operator/ for now

  Lazy &operator+=(const Lazy &arg) {
    this->m_factorization = std::nullopt;
    this->m_value += arg.m_value;
    return *this;
  }

  friend Lazy operator+(const Lazy &left, const Lazy &right) {
    Lazy out{left};
    out += right;
    return out;
  }

  Lazy &operator-=(const Lazy &arg) {
    this->m_factorization = std::nullopt;
    this->m_value -= arg.m_value;
    return *this;
  }

  friend Lazy operator-(const Lazy &left, const Lazy &right) {
    Lazy out{left};
    out -= right;
    return out;
  }

  ValueType get_value() const { return m_value; }
};

// `std::swap`-esque
// TODO-think: should there be lazy&& version?
template <typename Traits> auto &&factorize(const Lazy<Traits> &lazy) {
  return lazy.get_factorization();
}

template <typename Traits> auto &&value(const Lazy<Traits> &lazy) {
  return lazy.get_value();
}

} // namespace ivl::nt
