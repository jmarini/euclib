/*
 *	Copyright (C) 2010-2011 Jonathan Marini
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef EUBLIB_VECTOR_EXPRESSION_HPP
#define EUBLIB_VECTOR_EXPRESSION_HPP

#include <cstddef>	// for std::size_t
#include <boost/mpl/if.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/sizeof.hpp>

#include "type_traits.hpp"

/*
 * This class implements expression templates for efficient vector/matrix math
 *   i.e.  3 * ( v1 + v2 )
 *
 * References
 *   [1] D. Vandevoorde, N.M. Josuttis. C++ Templates: The Complete Guide.
 *         Crawfordsville, IN: Pearson Education Inc., 2003, pp. 321-344.
 *   [2] J. Härdtlein, C. Pflaum, A. Linke, C.H. Wolters. "Advanced expression templates programming".
 *         Computing and Visualization in Science, vol. 13, no. 2, pp. 59-68, 2010.
 */


namespace euclib {

////////////////////////////////////////
// Hiding some template metaprogramming
//   for use in expression templates

namespace mpl {
	template<typename T1, typename T2>
	struct promotion_ {
		static_assert( (std::is_arithmetic<T1>::value || mpl::is_decimal<T1>::value) &&
		               (std::is_arithmetic<T2>::value || mpl::is_decimal<T2>::value),
		               "T1 and T2 must be integral, floating point, or decimal types" );

		// Choose the type that is bigger when promoting
		typedef typename boost::mpl::if_< boost::mpl::less_equal< boost::mpl::sizeof_<T1>,
		                                                          boost::mpl::sizeof_<T2>
		                                                        >,
		                                  T2,
		                                  T1
		                                >::type type;
	};

	template<typename T>
	struct promotion_<T,T> { typedef T type; };

} // End namespace mpl


template<typename T>
struct expression_holder {
// Typedefs
	typedef T expression_t;

// Operators
	operator const T& ( ) const {
		return *static_cast<const T*>( this );
	}
private:
	const expression_holder& operator = ( const expression_holder& );

// Constructors
protected:
	expression_holder( ) { }
	virtual ~expression_holder( ) { }
};

////////////////////////////////////////
// Scalar type encapsulating
//   floating point types

template<typename T>
class scalar : public expression_holder<scalar<T>> {
	static_assert( std::is_floating_point<T>::value || mpl::is_decimal<T>::value,
	               "T must be floating point or decimal" );
// Variables
	const T m_value;

// Typedefs
public:
	typedef T            value_t;
	typedef std::size_t  size_t;

// Constructors
	scalar( const T& v ) : m_value( v ) { }

// Operators
	value_t operator [] ( size_t ) const { return m_value; }
};

template<typename T>
struct container { typedef const T& ref_t; };

template<typename T>
struct container<scalar<T>> { typedef scalar<T> ref_t; };


////////////////////////////////////////
// Expr + Expr addition

template<typename L, typename R>
class vector_addition : public expression_holder<vector_addition<L,R>> {
// Variables
	typename container<L>::ref_t m_lhs;
	typename container<R>::ref_t m_rhs;

// Typedefs
public:
	typedef typename mpl::promotion_<typename L::value_t,
	                                 typename R::value_t>::type value_t;
	typedef typename mpl::promotion_<typename L::size_t,
	                                 typename R::size_t>::type  size_t;

// Constructors
	vector_addition( const L& lhs, const R& rhs ) : m_lhs( lhs ), m_rhs( rhs ) { }

// Operators
	value_t operator [] ( size_t i ) const {
		return m_lhs[i] + m_rhs[i];
	}
};

template<typename L, typename R>
inline vector_addition<L,R> operator + ( const expression_holder<L>& lhs, const expression_holder<R>& rhs ) {
	return vector_addition<L,R>( lhs, rhs );
}


////////////////////////////////////////
// Expr - Expr subtraction

template<typename L, typename R>
class vector_subtraction : public expression_holder<vector_subtraction<L,R>> {
// Variables
	typename container<L>::ref_t m_lhs;
	typename container<R>::ref_t m_rhs;

// Typedefs
public:
	typedef typename mpl::promotion_<typename L::value_t,
	                                 typename R::value_t>::type value_t;
	typedef typename mpl::promotion_<typename L::size_t,
	                                 typename R::size_t>::type  size_t;

// Constructors
	vector_subtraction( const L& lhs, const R& rhs ) : m_lhs( lhs ), m_rhs( rhs ) { }

// Operators
	value_t operator [] ( size_t i ) const {
		return m_lhs[i] - m_rhs[i];
	}
};

template<typename L, typename R>
inline vector_subtraction<L,R> operator - ( const expression_holder<L>& lhs, const expression_holder<R>& rhs ) {
	return vector_subtraction<L,R>( lhs, rhs );
}


////////////////////////////////////////
// Expr * Expr multiplication
// Expr * Scalar multiplication
// Scalar * Expr multiplication

template<typename L, typename R>
class vector_multiplication : public expression_holder<vector_multiplication<L,R>> {
// Variables
	typename container<L>::ref_t m_lhs;
	typename container<R>::ref_t m_rhs;

// Typedefs
public:
	typedef typename mpl::promotion_<typename L::value_t,
	                                 typename R::value_t>::type value_t;
	typedef typename mpl::promotion_<typename L::size_t,
	                                 typename R::size_t>::type  size_t;

// Constructors
	vector_multiplication( const L& lhs, const R& rhs ) : m_lhs( lhs ), m_rhs( rhs ) { }

// Operators
	value_t operator [] ( size_t i ) const { return m_lhs[i] * m_rhs[i]; }
};

template<typename L, typename R>
inline vector_multiplication<L,R> operator * ( const expression_holder<L>& lhs,
                                               const expression_holder<R>& rhs ) {
	return vector_multiplication<L,R>( lhs, rhs );
}

// Scalar multiplication
// TODO: convert to typename L::value_t if not floating or decimal
template<typename L, typename R>
inline
typename std::enable_if< (std::is_floating_point<L>::value || mpl::is_decimal<L>::value),
                       vector_multiplication<scalar<L>,R>
					   >::type
operator * ( L lhs, const expression_holder<R>& rhs ) {
	return vector_multiplication<scalar<L>,R>( scalar<L>(lhs), rhs );
}

template<typename L, typename R>
inline
typename std::enable_if< (std::is_floating_point<R>::value || mpl::is_decimal<R>::value),
                       vector_multiplication<L,scalar<R>>
                       >::type
operator * ( const expression_holder<L>& lhs, R rhs ) {
	return vector_multiplication<L,scalar<R>>( lhs, scalar<R>(rhs) );
}


} // End namespace euclib

#endif // EUBLIB_VECTOR_EXPRESSION_HPP

