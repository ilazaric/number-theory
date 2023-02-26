#pragma once

#include <algorithm>
#include <compare>
#include <ostream>
#include <vector>

namespace ivl::nt {

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

    // `clang++-15 -Wsign-conversion`
    this->m_digits.insert(this->m_digits.end(),
      arg.m_digits.begin() + static_cast<typename std::vector<T>::iterator::difference_type>(limit),
      arg.m_digits.end());

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
      carry = 0;
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
    for (std::size_t i = 0; i < this->m_digits.size(); ++i) {
      for (std::size_t j = 0; j < arg.m_digits.size(); ++j) {
        std::size_t current = i + j;
        T carry = this->m_digits[i] * arg.m_digits[j];
        while (carry) {
          // if (current == tmp.size()) tmp.emplace_back();
          carry += tmp[current];
          tmp[current] = carry % Base;
          carry /= Base;
          ++current;
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

    std::vector<UnsignedBignum> multiples{ arg };
    std::vector<T> coefs{ 1 };
    while (coefs.back() < Base) {
      multiples.push_back(multiples.back() + multiples.back());
      coefs.push_back(coefs.back() + coefs.back());
    }
    std::reverse(multiples.begin(), multiples.end());
    std::reverse(coefs.begin(), coefs.end());

    UnsignedBignum acc;
    acc.m_digits.resize(this->m_digits.size() - arg.m_digits.size() + 1);
    for (auto &multiple : multiples) multiple.m_digits.insert(multiple.m_digits.begin(), acc.m_digits.size(), T{});
    for (std::size_t shift = this->m_digits.size() - arg.m_digits.size(); shift != static_cast<std::size_t>(-1);
         --shift) {
      for (auto &multiple : multiples) multiple.m_digits.erase(multiple.m_digits.begin());
      for (std::size_t i = 0; i < multiples.size(); ++i) {
        if (multiples[i] <= *this) {
          acc.m_digits[shift] += coefs[i];
          *this -= multiples[i];
        }
      }
    }

    while (!acc.m_digits.empty() && acc.m_digits.back() == 0) acc.m_digits.pop_back();
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
      // `clang++-15 -Wsign-conversion` forces me to do this
      out << arg.m_digits.rbegin()[static_cast<typename std::vector<T>::iterator::difference_type>(i)];
    }
    return out;
  }

  explicit UnsignedBignum(auto arg)
  {
    while (arg) {
      this->m_digits.emplace_back(static_cast<T>(arg % Base));
      arg /= Base;
    }
  }

  template<typename CT> CT get() const
  {
    CT out{};
    for (auto it = m_digits.rbegin(); it != m_digits.rend(); ++it)
      out = out * static_cast<CT>(Base) + static_cast<CT>(*it);
    return out;
  }
};

}// namespace ivl::nt
