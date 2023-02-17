#pragma once

#include <iosfwd>
#include <type_traits>

namespace ivl::nt {

class AddOverflowException : public std::exception
{
  const char *what() const noexcept override { return "attempt at addition resulted in overflow"; }
};

class SubOverflowException : public std::exception
{
  const char *what() const noexcept override { return "attempt at subtraction resulted in overflow"; }
};

class MulOverflowException : public std::exception
{
  const char *what() const noexcept override { return "attempt at multiplication resulted in overflow"; }
};

class DivInvalidException : public std::exception
{
  const char *what() const noexcept override
  {
    return "division is invalid unless numerator is divisible by denominator";
  }
};

template<typename T> class Safe
{
  static_assert(std::is_integral_v<T>, "for now only supporting builtin integral types");

private:
  T m_data;

public:
  constexpr Safe(T data) : m_data(data) {}
  constexpr Safe() : m_data{} {}

  constexpr T get() const { return m_data; }

  constexpr Safe operator+(Safe arg) const
  {
    Safe out;
    if (__builtin_add_overflow(this->m_data, arg.m_data, &out.m_data)) throw AddOverflowException{};
    return out;
  }

  constexpr Safe operator-(Safe arg) const
  {
    Safe out;
    if (__builtin_sub_overflow(this->m_data, arg.m_data, &out.m_data)) throw SubOverflowException{};
    return out;
  }

  constexpr Safe operator*(Safe arg) const
  {
    Safe out;
    if (__builtin_mul_overflow(this->m_data, arg.m_data, &out.m_data)) throw MulOverflowException{};
    return out;
  }

  constexpr Safe operator%(Safe arg) const { return Safe{ this->m_data % arg.m_data }; }

  constexpr Safe operator/(Safe arg) const
  {
    Safe out{ this->m_data / arg.m_data };
    if (out.m_data * arg.m_data != this->m_data) throw DivInvalidException{};
    return out;
  }

  friend constexpr auto operator<=>(Safe a, Safe b) = default;

  constexpr Safe &operator+=(Safe arg) { return *this = *this + arg; }
  constexpr Safe &operator-=(Safe arg) { return *this = *this - arg; }
  constexpr Safe &operator*=(Safe arg) { return *this = *this * arg; }
  constexpr Safe &operator%=(Safe arg) { return *this = *this % arg; }
  constexpr Safe &operator/=(Safe arg) { return *this = *this / arg; }

  Safe operator-() const { return Safe{ 0 } - *this; }

  Safe &operator++() { return *this += Safe{ 1 }; }

  // TODO: ++, --, --

  friend std::ostream &operator<<(std::ostream &out, Safe s) { return out << s.m_data; }
};

}// namespace ivl::nt
