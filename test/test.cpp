#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <ivl/bignum.hpp>
#include <limits>

template<typename T> void test_add()
{
  srand(42);
  for (std::uint32_t i = 0; i < 2000; ++i) {
    auto a = rand() % 1000000000;
    auto b = rand() % 1000000000;
    auto c = a + b;
    T at{ a };
    T bt{ b };
    T ct = at + bt;
    if (c != ct.template get<std::int64_t>()) {
      std::cout << "a:  " << a << std::endl;
      std::cout << "b:  " << b << std::endl;
      std::cout << "c:  " << c << std::endl;
      std::cout << "at:  " << at << std::endl;
      std::cout << "bt:  " << bt << std::endl;
      std::cout << "ct:  " << ct << std::endl;
      throw 42;
    }
  }
}

template<typename T> void test_sub()
{
  srand(42);
  for (std::uint32_t i = 0; i < 2000; ++i) {
    auto a = rand() % 1000000000;
    auto b = rand() % 1000000000;
    auto c = a - b;
    T at{ a };
    T bt{ b };
    T ct = at - bt;
    if (c != ct.template get<std::int64_t>()) {
      std::cout << "a:  " << a << std::endl;
      std::cout << "b:  " << b << std::endl;
      std::cout << "c:  " << c << std::endl;
      std::cout << "at:  " << at << std::endl;
      std::cout << "bt:  " << bt << std::endl;
      std::cout << "ct:  " << ct << std::endl;
      throw 42;
    }
  }
}

template<typename T> void test_div()
{
  srand(42);
  for (std::uint32_t i = 0; i < 2000; ++i) {
    auto a = rand() % 1000000000;
    auto b = rand() % 1000000000;
    auto c = (std::int64_t)a / b;
    T at{ a };
    T bt{ b };
    T ct = at / bt;
    if (c != ct.template get<std::int64_t>()) {
      std::cout << "a:  " << a << std::endl;
      std::cout << "b:  " << b << std::endl;
      std::cout << "c:  " << c << std::endl;
      std::cout << "at:  " << at << std::endl;
      std::cout << "bt:  " << bt << std::endl;
      std::cout << "ct:  " << ct << std::endl;
      throw 42;
    }
  }
}
template<typename T> void test_mul()
{
  srand(42);
  for (std::uint32_t i = 0; i < 2000; ++i) {
    auto a = rand() % 1000000000;
    auto b = rand() % 1000000000;
    auto c = (std::int64_t)a * b;
    T at{ a };
    T bt{ b };
    T ct = at * bt;
    if (c != ct.template get<std::int64_t>()) {
      std::cout << "a:  " << a << std::endl;
      std::cout << "b:  " << b << std::endl;
      std::cout << "c:  " << c << std::endl;
      std::cout << "at:  " << at << std::endl;
      std::cout << "bt:  " << bt << std::endl;
      std::cout << "ct:  " << ct << std::endl;
      throw 42;
    }
  }
}

template<typename T> T complicated_expression()
{
  // T a{10};
  // T b{0};
  // T c{1};
  T a{ 12901 };
  T b{ 39223 };
  T c{ 910129 };
  for (std::uint32_t i = 0; i < 20; ++i) {
    c = c * a + b;
    std::cout << "c[" << i << "]: " << c << std::endl;
  }
  return c;
}

template<typename T, T Mod> struct Mint
{
  T m_data;

  Mint &operator+=(Mint arg)
  {
    m_data += arg.m_data;
    m_data %= Mod;
    return *this;
  }
  friend Mint operator+(Mint a, Mint b)
  {
    a += b;
    return a;
  }

  Mint &operator-=(Mint arg)
  {
    m_data -= arg.m_data;
    m_data %= Mod;
    if (m_data < 0) m_data += Mod;
    return *this;
  }
  friend Mint operator-(Mint a, Mint b)
  {
    a -= b;
    return a;
  }

  Mint &operator*=(Mint arg)
  {
    m_data *= arg.m_data;
    m_data %= Mod;
    return *this;
  }
  friend Mint operator*(Mint a, Mint b)
  {
    a *= b;
    return a;
  }
};

template<typename T, T Mod> std::ostream &operator<<(std::ostream &out, Mint<T, Mod> m) { return out << m.m_data; }

template<typename T> void test1()
{
  constexpr std::int64_t P = 102931;
  // constexpr std::int64_t P = std::numeric_limits<std::int64_t>::max();
  auto a = complicated_expression<Mint<std::int64_t, P>>();
  auto b = complicated_expression<T>();
  auto av = a.m_data;
  auto bv = (b % T{ P }).template get<std::int64_t>();
  if (av != bv) {
    std::cout << "ERROR: " << av << " != " << bv << std::endl;
    std::cout << "a: " << a.m_data << std::endl;
    std::cout << "b: " << b << std::endl;
    exit(1);
  }
}

template<typename T> void multitest()
{
  test_add<T>();
  test_sub<T>();
  test_mul<T>();
  test_div<T>();
  test1<T>();
}

int main()
{
  multitest<ivl::nt::Bignum<std::int32_t, 10>>();
  multitest<ivl::nt::Bignum<std::int32_t, 10000>>();
  // multitest<ivl::nt::Bignum<std::int16_t, 10>>();
  multitest<ivl::nt::Bignum<std::int32_t, 2>>();
  return 0;
}
