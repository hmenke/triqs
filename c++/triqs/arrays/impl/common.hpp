// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

#ifndef TRIQS_ARRAYS_IMPL_COMMON_H
#define TRIQS_ARRAYS_IMPL_COMMON_H
#include <triqs/utility/first_include.hpp>
#include <triqs/clef.hpp>

/// Maximum dimension of the arrays
#define ARRAY_NRANK_MAX 10

#include <exception>
#include <assert.h>
#include <triqs/utility/exceptions.hpp>
#include <sstream>
#include <type_traits>

template <char C> using char_ = std::integral_constant<char, C>;

#include <triqs/utility/is_complex.hpp>
#include <triqs/utility/macros.hpp>
#include <triqs/arrays/indexmaps/cuboid/mem_layout.hpp>
#include "./traits.hpp"

namespace boost {
  namespace serialization {
    class access;
  }
} // namespace boost

#ifdef TRIQS_DEBUG
#define TRIQS_ARRAYS_DEBUG
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#endif

namespace triqs {

  typedef unsigned long long ull_t;

  /// Makes a view
  template <typename A> typename A::view_type make_view(A const &x) { return typename A::view_type(x); }
  template <typename A> typename A::const_view_type make_const_view(A const &x) { return typename A::const_view_type(x); }

  //
  //template<class, class = std::void_t<>> struct has_regular_type: std::false_type {};
  //template<class T> struct has_regular_type<T, std::void_t<typename T::regular_type>> : std::true_type {};

  /// Makes a clone
  //template<typename A> auto make_clone(A const & x) {
  //if  constexpr (has_regular_type<A>::value) {
  //  return typename A::regular_type(x);}
  // else {
  //  return A{x};
  //  }
  //}

  /// Makes a clone
  template <typename T, typename = std::void_t<>> struct _make_clone {
    static T invoke(T const &x) { return T{x}; }
  };
  template <typename T> struct _make_clone<T, std::void_t<typename T::regular_type>> {
    static auto invoke(T const &x) { return typename T::regular_type{x}; }
  };
  template <typename T> auto make_clone(T const &x) { return _make_clone<T>::invoke(x); }

  namespace arrays {
    using triqs::make_clone;

    /// Is the data contiguous
    template <typename A> TYPE_DISABLE_IFC(bool, is_amv_value_or_view_class<A>::value) has_contiguous_data(A const &) { return false; }
    template <typename A> TYPE_ENABLE_IFC(bool, is_amv_value_class<A>::value) has_contiguous_data(A const &) { return true; }
    template <typename A> TYPE_ENABLE_IFC(bool, is_amv_view_class<A>::value) has_contiguous_data(A const &v) { return v.indexmap().is_contiguous(); }

    template <typename A> ENABLE_IF(is_amv_view_class<A>) resize_or_check_if_view(A &a, typename A::shape_type const &sha) {
      if (a.shape() != sha) TRIQS_RUNTIME_ERROR << "Size mismatch : view class shape = " << a.shape() << " expected " << sha;
    }

    template <typename A> ENABLE_IF(is_amv_value_class<A>) resize_or_check_if_view(A &a, typename A::shape_type const &sha) {
      if (a.shape() != sha) a.resize(sha);
    }

    /// Check shape and memory layout
    template <typename A>
    ENABLE_IF(is_amv_view_class<A>)
    resize_or_check_if_view(A &a, typename A::shape_type const &sha, memory_layout_t<A::domain_type::rank> const &memory_layout) {
      resize_or_check_if_view(a, sha);
      if (a.memory_layout() != memory_layout)
        TRIQS_RUNTIME_ERROR << "Memory layout mismatch : view class memory layout = " << a.memory_layout() << ", expected " << memory_layout;
    }

    /// Resize and update memory layout
    template <typename A>
    ENABLE_IF(is_amv_value_class<A>)
    resize_or_check_if_view(A &a, typename A::shape_type const &sha, memory_layout_t<A::domain_type::rank> const &memory_layout) {
      if (a.shape() != sha || a.memory_layout() != memory_layout) a.resize(sha, memory_layout);
    }

  } // namespace arrays
} // namespace triqs
#endif
