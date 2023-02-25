#pragma once

#include <compare>
#include <ostream>
#include <type_traits>

namespace ivl::nt {

// creates a signed type from an unsigned one
template<typename T> class SignedWrapper
{
private:
  enum class Sign : char { Negative = -1, Zero = 0, Positive = +1 };
  Sign m_sign;
  T m_abs;

  bool is_zero() const { return m_sign == Sign::Zero; }
  bool is_positive() const { return m_sign == Sign::Positive; }
  bool is_negative() const { return m_sign == Sign::Negative; }

  SignedWrapper(Sign sign, T abs) : m_sign(sign), m_abs(abs) {}

public:
  SignedWrapper() : m_sign(Sign::Zero), m_abs() {}

  template<typename CT> CT get() const
  {
    CT out = this->m_abs.template get<CT>();
    return this->is_negative() ? -out : +out;
  }

  // use of `std::make_unsigned_t` is a bit annoying
  // cant specialize it :(
  // TODO-think
  template<typename U>
  SignedWrapper(U arg)
    : m_sign(arg > U{}   ? Sign::Positive
             : arg < U{} ? Sign::Negative
                         : Sign::Zero),
      m_abs(static_cast<std::make_unsigned_t<U>>(arg > U{} ? arg : -arg))
  {}

  friend std::ostream &operator<<(std::ostream &out, const SignedWrapper &arg)
  {
    switch (arg.m_sign) {
    case Sign::Negative:
      out << '-';
      break;
    case Sign::Zero:
      out << '0';
      break;
    case Sign::Positive:
      break;
    }
    return out << arg.m_abs;
  }

  std::strong_ordering operator<=>(const SignedWrapper &arg) const
  {
    if (this->m_sign != arg.m_sign) { return this->m_sign <=> arg.m_sign; }
    if (this->is_zero()) { return std::strong_ordering::equal; }
    std::strong_ordering cmp = this->m_abs <=> arg.m_abs;
    if (this->is_positive()) {
      return cmp;
    } else {
      return 0 <=> cmp;
    }
  }

  static Sign opposite(Sign sign) noexcept
  {
    switch (sign) {
    case Sign::Positive:
      return Sign::Negative;
    case Sign::Zero:
      return Sign::Zero;
    case Sign::Negative:
      return Sign::Positive;
    }
    exit(1);// this is impossible to reach, compiler is not smart enough to figure this out for some reason
  }

  SignedWrapper &operator+=(const SignedWrapper &arg)
  {
    if (arg.is_zero()) { return *this; }

    if (this->is_zero()) { return *this = arg; }

    if (this->m_sign == arg.m_sign) {
      this->m_abs += arg.m_abs;
      return *this;
    }

    std::strong_ordering cmp = this->m_abs <=> arg.m_abs;
    if (cmp == std::strong_ordering::less) {
      T tmp{ std::move(this->m_abs) };
      this->m_abs = arg.m_abs;
      this->m_abs -= std::move(tmp);
      this->m_sign = opposite(this->m_sign);
    } else if (cmp == std::strong_ordering::greater) {
      this->m_abs -= arg.m_abs;
    } else {// equal
      this->m_sign = Sign::Zero;
      m_abs = T{};
    }
    return *this;
  }

  friend SignedWrapper operator+(const SignedWrapper &left, const SignedWrapper &right)
  {
    SignedWrapper out{ left };
    out += right;
    return out;
  }

  SignedWrapper &operator-=(const SignedWrapper &arg)
  {
    if (arg.is_zero()) { return *this; }

    if (this->is_zero()) { return *this = SignedWrapper{ opposite(arg.m_sign), arg.m_abs }; }

    if (this->m_sign != arg.m_sign) {
      this->m_abs += arg.m_abs;
      return *this;
    }

    std::strong_ordering cmp = this->m_abs <=> arg.m_abs;
    if (cmp == std::strong_ordering::less) {
      T tmp{ std::move(this->m_abs) };
      this->m_abs = arg.m_abs;
      this->m_abs -= std::move(tmp);
      this->m_sign = opposite(this->m_sign);
    } else if (cmp == std::strong_ordering::greater) {
      this->m_abs -= arg.m_abs;
    } else {// equal
      this->m_sign = Sign::Zero;
      m_abs = T{};
    }
    return *this;
  }

  friend SignedWrapper operator-(const SignedWrapper &left, const SignedWrapper &right)
  {
    SignedWrapper out{ left };
    out -= right;
    return out;
  }

  SignedWrapper &operator*=(const SignedWrapper &arg)
  {
    if (this->is_zero()) { return *this; }

    if (arg.is_zero()) { return *this = arg; }

    this->m_abs *= arg.m_abs;
    if (arg.is_negative()) { this->m_sign = opposite(this->m_sign); }
    return *this;
  }

  friend SignedWrapper operator*(const SignedWrapper &left, const SignedWrapper &right)
  {
    SignedWrapper out{ left };
    out *= right;
    return out;
  }

  class WrapperDivZeroException : public std::exception
  {
  public:
    const char *what() const noexcept override { return "tried to divide by zero"; }
  };

  SignedWrapper &operator/=(const SignedWrapper &arg)
  {
    if (arg.is_zero()) { throw WrapperDivZeroException{}; }

    if (this->is_zero()) { return *this; }

    this->m_abs /= arg.m_abs;
    if (arg.is_negative()) { this->m_sign = opposite(this->m_sign); }
    return *this;
  }

  friend SignedWrapper operator/(const SignedWrapper &left, const SignedWrapper &right)
  {
    SignedWrapper out{ left };
    out /= right;
    return out;
  }

  SignedWrapper &operator%=(const SignedWrapper &arg)
  {
    if (arg.is_zero()) { throw WrapperDivZeroException{}; }

    if (this->is_zero()) { return *this; }

    this->m_abs %= arg.m_abs;
    if (arg.is_negative()) { this->m_sign = opposite(this->m_sign); }
    if (this->m_abs == T{}) { this->m_sign = Sign::Zero; }
    return *this;
  }

  friend SignedWrapper operator%(const SignedWrapper &left, const SignedWrapper &right)
  {
    SignedWrapper out{ left };
    out %= right;
    return out;
  }
};

}// namespace ivl::nt
