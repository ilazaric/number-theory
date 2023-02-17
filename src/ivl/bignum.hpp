#pragma once

#include <compare>
#include <ostream>
#include <vector>

namespace ivl::nt {

std::strong_ordering reverse_order(std::strong_ordering arg) { return 0 <=> arg; }

// `T` should be chosen such that the expression:
// `Base * Base`
// fits entirely in `T`, no overflow

// all operations on this have to remain nonnegative
// as in, UnsignedBignum - BiggerUnsignedBignum = throw
template<typename T, T Base> class UnsignedBignum
{
  static_assert(Base > 1);
  static_assert(T{ Base * Base } / Base == Base,
    "Type `T` must be capable of fully containing the expression `Base * Base`");

private:
  std::vector<T> m_digits;

public:
  std::strong_ordering operator<=>(const UnsignedBignum &arg) const
  {
    std::strong_ordering result = std::strong_ordering::equal;
    if (result = this->m_digits.size() <=> arg.m_digits.size(); result != std::strong_ordering::equal) {
      return result;
    }

    std::size_t i = this->m_digits.size();
    while (i) {
      --i;
      if (result = this->m_digits[i] <=> arg.m_digits[i]; result != std::strong_ordering::equal) { return result; }
    }

    return std::strong_ordering::equal;// == result
  }

  friend bool operator==(const UnsignedBignum &left, const UnsignedBignum &right) { return (left <=> right) == 0; }

  friend bool operator!=(const UnsignedBignum &left, const UnsignedBignum &right) { return (left <=> right) != 0; }

  UnsignedBignum &operator+=(const UnsignedBignum &arg)
  {
    T carry{ 0 };
    std::size_t limit = std::min(this->m_digits.size(), arg.m_digits.size());

    this->m_digits.insert(this->m_digits.end(), arg.m_digits.begin() + limit, arg.m_digits.end());

    std::size_t i = 0;
    for (; i < limit; ++i) {
      carry += this->m_digits[i];
      carry += arg.m_digits[i];
      if (carry < Base) {
        this->m_digits[i] = carry;
        carry = 0;
      } else {
        this->m_digits[i] = carry - Base;
        carry = 1;
      }
    }

    for (; carry && i < this->m_digits.size(); ++i) {
      ++this->m_digits[i];
      if (this->m_digits[i] == Base) {
        this->m_digits[i] = 0;
        carry = 1;
      }
    }

    if (carry) { this->m_digits.emplace_back(carry); }

    return *this;
  }

  friend UnsignedBignum operator+(const UnsignedBignum &left, const UnsignedBignum &right)
  {
    // here i pray NRVO works
    UnsignedBignum out{ left };
    out += right;
    return out;
  }

  class SubInvalidException : public std::exception
  {
    const char *what() const noexcept override
    {
      return "tried to subtract UnsignedBignum values where the result would have to be negative; not okay";
    }
  };

  UnsignedBignum &operator-=(const UnsignedBignum &arg)
  {
    if (*this < arg) { throw SubInvalidException{}; }

    T carry{ 0 };
    for (std::size_t i = 0; i < arg.m_digits.size(); ++i) {
      carry += arg.m_digits[i];
      if (this->m_digits[i] < carry) {
        this->m_digits[i] += Base - carry;
        carry = 1;
      } else {
        this->m_digits[i] -= carry;
        carry = 0;
      }
    }

    for (std::size_t i = arg.m_digits.size(); carry != T{ 0 } && i < this->m_digits.size(); ++i) {
      if (this->m_digits[i] == T{ 0 }) {
        this->m_digits[i] = Base - 1;
        carry = 1;
      } else {
        --this->m_digits[i];
        carry = 0;
      }
    }

    while (!this->m_digits.empty() && this->m_digits.back() == T{ 0 }) { this->m_digits.pop_back(); }

    return *this;
  }

  friend UnsignedBignum operator-(const UnsignedBignum &left, const UnsignedBignum &right)
  {
    UnsignedBignum out{ left };
    out -= right;
    return out;
  }

  // stupid slow implementation for now
  UnsignedBignum &operator*=(const UnsignedBignum &arg)
  {
    if (arg.m_digits.empty()) { this->m_digits.clear(); }
    if (this->m_digits.empty()) { return *this; }

    std::vector<T> tmp(this->m_digits.size() + arg.m_digits.size());
    for (std::size_t diag = 0; diag + 1 < this->m_digits.size() + arg.m_digits.size(); ++diag) {
      for (std::size_t i = diag < arg.m_digits.size() ? 0 : diag + 1 - arg.m_digits.size(),
                       j = diag - i,
                       limit = std::min(this->m_digits.size(), diag + 1);
           i < limit;
           ++i, --j) {
        T current = this->m_digits[i] * arg.m_digits[j];
        for (std::size_t index = diag; current; ++index, current /= Base) {
          if (tmp.size() == index) { tmp.emplace_back(T{}); }
          current += tmp[index];
          tmp[index] = current % Base;
        }
      }
    }

    while (tmp.back() == T{}) tmp.pop_back();

    this->m_digits = std::move(tmp);
    return *this;
  }

  friend UnsignedBignum operator*(const UnsignedBignum &left, const UnsignedBignum &right)
  {
    UnsignedBignum out{ left };
    out *= right;
    return out;
  }

  UnsignedBignum() {}

  UnsignedBignum &operator/=(const UnsignedBignum &arg)
  {
    if (this->m_digits.size() < arg.m_digits.size()) {
      this->m_digits.clear();
      return *this;
    }

    if (arg.m_digits.empty()) {
      throw 12;// TODO
    }

    UnsignedBignum acc;
    acc.m_digits.resize(this->m_digits.size() - arg.m_digits.size() + 1);
    for (std::size_t shift = this->m_digits.size() - arg.m_digits.size(); shift != static_cast<std::size_t>(-1);
         --shift) {
      T lo{ 0 }, hi{ Base - 1 };
      for (T mid{ (lo + hi + 1) / 2 }; lo != hi; mid = (lo + hi + 1) / 2) {
        UnsignedBignum tmp;
        tmp.m_digits = { mid };
        tmp *= arg;
        tmp.m_digits.insert(tmp.m_digits.begin(), shift, T{});
        if (tmp <= *this)
          lo = mid;
        else
          hi = mid - 1;
      }
      if (lo) {
        UnsignedBignum tmp;
        tmp.m_digits = { lo };
        tmp *= arg;
        tmp.m_digits.insert(tmp.m_digits.begin(), shift, T{});
        *this -= tmp;
        acc.m_digits[shift] = lo;
      }
    }

    while (acc.m_digits.back() == 0) acc.m_digits.pop_back();
    return *this = acc;
  }

  friend UnsignedBignum operator/(const UnsignedBignum &left, const UnsignedBignum &right)
  {
    UnsignedBignum out{ left };
    out /= right;
    return out;
  }

  UnsignedBignum &operator%=(const UnsignedBignum &arg) { return *this -= *this / arg * arg; }

  friend UnsignedBignum operator%(const UnsignedBignum &left, const UnsignedBignum &right)
  {
    UnsignedBignum out{ left };
    out %= right;
    return out;
  }

  friend std::ostream &operator<<(std::ostream &out, const UnsignedBignum &arg)
  {
    for (std::size_t i = 0; i < arg.m_digits.size(); ++i) {
      if (i) out << '_';
      out << arg.m_digits.rbegin()[i];
    }
    return out;
  }

  explicit UnsignedBignum(auto arg)
  {
    while (arg) {
      this->m_digits.emplace_back(T(arg % Base));
      arg /= Base;
    }
  }
};

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

  template<typename U>
  SignedWrapper(U arg)
    : m_sign(arg > U{}   ? Sign::Positive
             : arg < U{} ? Sign::Negative
                         : Sign::Zero),
      m_abs(arg > U{} ? arg : -arg)
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
      return reverse_order(cmp);
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

template<typename T, T Base> using Bignum = SignedWrapper<UnsignedBignum<T, Base>>;

}// namespace ivl::nt
