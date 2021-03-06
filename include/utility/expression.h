//
//  Expression.h
//  Evaluates string expression using Boost Spirit
//
//  Created by Adel Ahmadyan on 11/13/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//
#pragma once

//--------------------------------------------------------------------------
//
// Copyright (C) 2011, 2012, 2013 Rhys Ulerich
// Copyright (C) 2012, 2013 The PECOS Development Team
// Please see http://pecos.ices.utexas.edu for more information on PECOS.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//--------------------------------------------------------------------------

#include <cmath>
#include <iterator>
#include <limits>

#include <boost/spirit/version.hpp>
#if !defined(SPIRIT_VERSION) || SPIRIT_VERSION < 0x2010
#error "At least Spirit version 2.1 required"
#endif
#include <boost/math/constants/constants.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace expression {

namespace { // anonymous

struct lazy_pow_ {
  template <typename X, typename Y> struct result { typedef X type; };

  template <typename X, typename Y> X operator()(X x, Y y) const {
    return std::pow(x, y);
  }
};

struct lazy_ufunc_ {
  template <typename F, typename A1> struct result { typedef A1 type; };

  template <typename F, typename A1> A1 operator()(F f, A1 a1) const {
    return f(a1);
  }
};

struct lazy_bfunc_ {
  template <typename F, typename A1, typename A2> struct result {
    typedef A1 type;
  };

  template <typename F, typename A1, typename A2>
  A1 operator()(F f, A1 a1, A2 a2) const {
    return f(a1, a2);
  }
};

template <class T> T max_by_value(const T a, const T b) {
  return std::max(a, b);
}

template <class T> T min_by_value(const T a, const T b) {
  return std::min(a, b);
}

} // namespace

template <typename FPT, typename Iterator>
struct grammar : boost::spirit::qi::grammar<Iterator, FPT(),
                                            boost::spirit::ascii::space_type> {

  // symbol table for constants like "pi"
  struct constant_
      : boost::spirit::qi::symbols<
            typename std::iterator_traits<Iterator>::value_type, FPT> {
    constant_() {
      this->add("digits", std::numeric_limits<FPT>::digits)(
          "digits10", std::numeric_limits<FPT>::digits10)(
          "e", boost::math::constants::e<FPT>())(
          "epsilon", std::numeric_limits<FPT>::epsilon())(
          "pi", boost::math::constants::pi<FPT>());
    }
  } constant;

  // symbol table for unary functions like "abs"
  struct ufunc_
      : boost::spirit::qi::symbols<
            typename std::iterator_traits<Iterator>::value_type, FPT (*)(FPT)> {
    ufunc_() {
      this->add("abs", static_cast<FPT (*)(FPT)>(&std::abs))(
          "acos", static_cast<FPT (*)(FPT)>(&std::acos))(
          "asin", static_cast<FPT (*)(FPT)>(&std::asin))(
          "atan", static_cast<FPT (*)(FPT)>(&std::atan))(
          "ceil", static_cast<FPT (*)(FPT)>(&std::ceil))(
          "cos", static_cast<FPT (*)(FPT)>(&std::cos))(
          "cosh", static_cast<FPT (*)(FPT)>(&std::cosh))(
          "exp", static_cast<FPT (*)(FPT)>(&std::exp))(
          "floor", static_cast<FPT (*)(FPT)>(&std::floor))(
          "log10", static_cast<FPT (*)(FPT)>(&std::log10))(
          "log", static_cast<FPT (*)(FPT)>(&std::log))(
          "sin", static_cast<FPT (*)(FPT)>(&std::sin))(
          "sinh", static_cast<FPT (*)(FPT)>(&std::sinh))(
          "sqrt", static_cast<FPT (*)(FPT)>(&std::sqrt))(
          "tan", static_cast<FPT (*)(FPT)>(&std::tan))(
          "tanh", static_cast<FPT (*)(FPT)>(&std::tanh));
    }
  } ufunc;

  // symbol table for binary functions like "pow"
  struct bfunc_ : boost::spirit::qi::symbols<
                      typename std::iterator_traits<Iterator>::value_type,
                      FPT (*)(FPT, FPT)> {
    bfunc_() {
      this->add("atan2", static_cast<FPT (*)(FPT, FPT)>(&std::atan2))(
          "max", static_cast<FPT (*)(FPT, FPT)>(&max_by_value))(
          "min", static_cast<FPT (*)(FPT, FPT)>(&min_by_value))(
          "pow", static_cast<FPT (*)(FPT, FPT)>(&std::pow));
    }
  } bfunc;

  boost::spirit::qi::rule<Iterator, FPT(), boost::spirit::ascii::space_type>
      expression, term, factor, primary;

  grammar() : grammar::base_type(expression) {
    using boost::spirit::qi::real_parser;
    using boost::spirit::qi::real_policies;
    real_parser<FPT, real_policies<FPT>> real;

    using boost::spirit::qi::_1;
    using boost::spirit::qi::_2;
    using boost::spirit::qi::_3;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::_val;

    boost::phoenix::function<lazy_pow_> lazy_pow;
    boost::phoenix::function<lazy_ufunc_> lazy_ufunc;
    boost::phoenix::function<lazy_bfunc_> lazy_bfunc;

    expression = term[_val = _1] >>
                 *(('+' >> term[_val += _1]) | ('-' >> term[_val -= _1]));

    term = factor[_val = _1] >>
           *(('*' >> factor[_val *= _1]) | ('/' >> factor[_val /= _1]));

    factor =
        primary[_val = _1] >> *(("**" >> factor[_val = lazy_pow(_val, _1)]));

    primary = real[_val = _1] | '(' >> expression[_val = _1] >> ')' |
              ('-' >> primary[_val = -_1]) | ('+' >> primary[_val = _1]) |
              (no_case[ufunc] >> '(' >> expression >>
               ')')[_val = lazy_ufunc(_1, _2)] |
              (no_case[bfunc] >> '(' >> expression >> ',' >> expression >>
               ')')[_val = lazy_bfunc(_1, _2, _3)] |
              no_case[constant][_val = _1];
  }
};

template <typename FPT, typename Iterator>
bool parse(Iterator &iter, Iterator end, const grammar<FPT, Iterator> &g,
           FPT &result) {
  return boost::spirit::qi::phrase_parse(iter, end, g,
                                         boost::spirit::ascii::space, result);
}

} // end namespace expression
