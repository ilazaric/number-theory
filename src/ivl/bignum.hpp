#pragma once

#include "bignum_details/signed_wrapper.hpp"
#include "bignum_details/unsigned_bignum.hpp"

namespace ivl::nt {

// `T` should be chosen such that the expression:
// `Base * Base`
// fits entirely in `T`, no overflow
// ^ verified in some `static_assert`
template<typename T, T Base> using Bignum = SignedWrapper<UnsignedBignum<T, Base>>;

}// namespace ivl::nt
