#pragma once

#include <algorithm>
#include <compare>
#include <ostream>
#include <span>
#include <vector>

namespace ivl::nt {

// all operations on this have to remain nonnegative
// as in, UnsignedBignum - BiggerUnsignedBignum = throw
// TODO: rvalue-ref versions
// TODO: create your own exceptions
template<typename T, T Base> class UnsignedBignum
{
  static_assert(Base > 1);
  static_assert(T{ Base * Base } / Base == Base,
    "Type `T` must be capable of fully containing the expression `Base * Base`");

  // TODO: stop being lazy
public:
  std::vector<T> m_digits;

public:
  // analogue of `std::string_view`
  // you can think of it as `const UnsignedBignum&`, though more powerful
  // useful when i want to do stuff with a substring of digits
  friend class View;
  class View
  {
    friend class UnsignedBignum;

  private:
    std::span<const T> m_digits;

  public:
    /*explicit*/ View(const UnsignedBignum &arg) : m_digits(arg.m_digits) {}
    View(const UnsignedBignum &arg, std::size_t lo, std::size_t hi) : m_digits(arg.m_digits.data() + lo, hi - lo) {}

    std::strong_ordering operator<=>(View arg) const;
    bool operator==(View arg) const { return (*this <=> arg) == 0; }
    bool operator!=(View arg) const { return (*this <=> arg) != 0; }
  };

  explicit UnsignedBignum(View arg) : m_digits(arg.m_digits.begin(), arg.m_digits.end()) {}

  // ordering stuff
  // avoiding `friend` bc it is real nasty to split definition from declaration
  // ^ https://www.daniweb.com/programming/software-development/threads/65196/undefined-reference-error-help#post291153
  std::strong_ordering operator<=>(const auto &arg) const { return View(*this) <=> arg; }
  bool operator==(const auto &arg) const { return (*this <=> arg) == 0; }
  bool operator!=(const auto &arg) const { return (*this <=> arg) != 0; }

  // arithmetic
  UnsignedBignum &operator+=(View arg);
  friend UnsignedBignum operator+(View left, View right)
  {
    UnsignedBignum out{ left };
    out += right;
    return out;
  }

  UnsignedBignum &operator-=(View arg);
  friend UnsignedBignum operator-(View left, View right)
  {
    UnsignedBignum out{ left };
    out -= right;
    return out;
  }

  UnsignedBignum &operator*=(View arg);
  friend UnsignedBignum operator*(View left, View right)
  {
    UnsignedBignum out{ left };
    out *= right;
    return out;
  }

  UnsignedBignum() {}

  UnsignedBignum &operator/=(View arg);
  friend UnsignedBignum operator/(View left, View right)
  {
    UnsignedBignum out{ left };
    out /= right;
    return out;
  }

  UnsignedBignum &operator%=(View arg) { return *this -= *this / arg * arg; }
  friend UnsignedBignum operator%(View left, View right)
  {
    UnsignedBignum out{ left };
    out %= right;
    return out;
  }

  // TODO: print Base as well
  friend std::ostream &operator<<(std::ostream &out, View arg)
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


// ordering
template<typename T, T Base> std::strong_ordering UnsignedBignum<T, Base>::View::operator<=>(View arg) const
{
  std::strong_ordering result = std::strong_ordering::equal;
  if (result = this->m_digits.size() <=> arg.m_digits.size(); result != std::strong_ordering::equal) { return result; }

  std::size_t i = this->m_digits.size();
  while (i) {
    --i;
    if (result = this->m_digits[i] <=> arg.m_digits[i]; result != std::strong_ordering::equal) { return result; }
  }

  return std::strong_ordering::equal;// == result
}


// arithmetic
template<typename T, T Base> UnsignedBignum<T, Base> &UnsignedBignum<T, Base>::operator+=(View arg)
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


template<typename T, T Base> UnsignedBignum<T, Base> &UnsignedBignum<T, Base>::operator-=(View arg)
{
  if (*this < arg) { throw std::runtime_error("UnsignedBignum: bad sub call"); }

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


template<typename T, T Base>
void shiftdiff(UnsignedBignum<T, Base> &left, std::size_t shift, const UnsignedBignum<T, Base> &right)
{
  T carry{ 0 };
  for (std::size_t i = 0; i < right.m_digits.size(); ++i) {
    carry += right.m_digits[i];
    if (left.m_digits[i + shift] < carry) {
      left.m_digits[i + shift] += Base - carry;
      carry = 1;
    } else {
      left.m_digits[i + shift] -= carry;
      carry = 0;
    }
  }

  for (std::size_t i = right.m_digits.size(); carry != T{ 0 } && i < left.m_digits.size(); ++i) {
    if (left.m_digits[i + shift] == T{ 0 }) {
      left.m_digits[i + shift] = Base - 1;
      carry = 1;
    } else {
      --left.m_digits[i + shift];
      carry = 0;
    }
  }

  while (!left.m_digits.empty() && left.m_digits.back() == T{ 0 }) { left.m_digits.pop_back(); }
}


// stupid slow implementation for now
template<typename T, T Base> UnsignedBignum<T, Base> &UnsignedBignum<T, Base>::operator*=(View arg)
{
  if (arg.m_digits.empty()) { this->m_digits.clear(); }
  if (this->m_digits.empty()) { return *this; }

  std::vector<T> tmp(this->m_digits.size() + arg.m_digits.size());
  for (std::size_t i = 0; i < this->m_digits.size(); ++i) {
    for (std::size_t j = 0; j < arg.m_digits.size(); ++j) { tmp[i + j] += this->m_digits[i] * arg.m_digits[j]; }
    for (std::size_t j = i; j + 1 < std::min(tmp.size(), i + arg.m_digits.size() + 1); ++j) {
      tmp[j + 1] += tmp[j] / Base;
      tmp[j] %= Base;
    }
  }

  // for (auto x : tmp) if (x >= Base) throw 42;

  while (tmp.back() == T{}) tmp.pop_back();

  this->m_digits = std::move(tmp);
  return *this;
}


template<typename T, T Base> UnsignedBignum<T, Base> &UnsignedBignum<T, Base>::operator/=(View arg)
{
  if (this->m_digits.size() < arg.m_digits.size()) {
    this->m_digits.clear();
    return *this;
  }

  if (arg.m_digits.empty()) {
    throw 12;// TODO
  }

  std::vector<UnsignedBignum> multiples{ UnsignedBignum(arg) };
  std::vector<T> coefs{ 1 };
  while (coefs.back() < Base) {
    multiples.push_back(multiples.back() + multiples.back());
    coefs.push_back(coefs.back() + coefs.back());
  }
  std::reverse(multiples.begin(), multiples.end());
  std::reverse(coefs.begin(), coefs.end());

  UnsignedBignum acc;
  acc.m_digits.resize(this->m_digits.size() - arg.m_digits.size() + 1);
  for (std::size_t shift = this->m_digits.size() - arg.m_digits.size(); shift != static_cast<std::size_t>(-1);
       --shift) {
    for (std::size_t i = 0; i < multiples.size(); ++i) {
      if (multiples[i] <= View(*this, shift, this->m_digits.size())) {
        acc.m_digits[shift] += coefs[i];
        shiftdiff(*this, shift, multiples[i]);
      }
    }
  }

  while (!acc.m_digits.empty() && acc.m_digits.back() == 0) acc.m_digits.pop_back();
  return *this = acc;
}

}// namespace ivl::nt
