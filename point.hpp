/*
 *	Copyright (C) 2010 Jonathan Marini
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

#ifndef EUBLIB_POINT_HPP
#define EUBLIB_POINT_HPP

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <array>

#include "euclib_math.hpp"

namespace euclib {

template<typename T, unsigned int D, typename INTERNAL = float>
class point_base {
// Typedefs
protected:

	typedef std::numeric_limits<T> limit_t;
	typedef INTERNAL               internal_t;

	// This class can only be used with integral or floating point types
	static_assert( boost::is_arithmetic<T>::value, "T must be an integral or floating point type" );
	static_assert( D != 0, "cannot have 0-dimensional object" );
	static_assert( boost::is_floating_point<internal_t>::value, "INTERNAL must be floating point" );


// Variables
protected:

	std::array<T,D> m_data;


// Constructors
protected: // cannot construct directly

	point_base( ) { }
	point_base( const point_base<T,D>& pt ) { *this = pt; }
	point_base( point_base<T,D>&& pt ) { *this = std::move( pt ); }
	// For use when constructing points from expressions
	template<typename Expr>
	point_base( const Expr& expr ) { evaluate( expr ); }
	// Limit number of arguments to size of point
	//   can have fewer, remaining spots filled with 0
	template<typename ... Args>
	point_base( T value, Args... values ) {
		static_assert( sizeof...(values) < D,
		               "too many arguments to constructor" );
		fill( 0, value, values... );
	}

// Methods
public:

	unsigned int dimension( ) const { return D; }


protected:

	template<typename ... Args>
	inline void fill( unsigned int i, T value, Args... values ) {
		m_data[i] = value;
		fill( i + 1, values... );
	}

	inline void fill( unsigned int i ) {
		for( ; i < D; ++i ) {
			m_data[i] = 0;
		}
	}

	template<typename Expr>
	inline void evaluate( const Expr& expr ) {
		static_assert( boost::is_base_of<expression,Expr>::value,
		               "invalid assignment or construction" );

		for( unsigned int i = 0; i < D; ++i ) {
			m_data[i] = expr[i];
		}
	}

// Operators
public:

	T operator [] ( unsigned int i ) const {
		assert( i < D );
		return m_data[i];
	}

	T& operator [] ( unsigned int i ) {
		assert( i < D );
		return m_data[i];
	}

	bool operator == ( const point_base<T,D>& pt ) const {
		for( unsigned int i = 0; i < D; ++i ) {
			if( !equal( m_data[i], pt[i] ) ) {
				return false;
			}
		}
		return true;
	}

	bool operator != ( const point_base<T,D>& pt ) const {
		return !(*this == pt);
	}

	point_base<T,D>& operator = ( const point_base<T,D>& pt ) {
		m_data = pt.m_data;
		return *this;
	}

	point_base<T,D>& operator = ( point_base<T,D>&& pt ) {
		std::swap( m_data, pt.m_data );
		return *this;
	}

	// For use when assigning points from expressions
	template<typename Expr>
	point_base<T,D>& operator = ( const Expr& expr ) {
		evaluate( expr );
		return *this;
	}

	template<typename R>
	point_base<T,D>& operator += ( const point_base<R,D>& pt ) {
		for( unsigned int i = 0; i < D; ++i ) {
			m_data[i] += static_cast<T>(pt[i]);
		}
		return *this;
	}

	template<typename R>
	point_base<T,D>& operator -= ( const point_base<R,D>& pt ) {
		for( unsigned int i = 0; i < D; ++i ) {
			m_data[i] -= static_cast<T>(pt[i]);
		}
		return *this;
	}

	point_base<T,D>& operator *= ( internal_t value ) {
		for( unsigned int i = 0; i < D; ++i ) {
			m_data[i] *= static_cast<T>(value);
		}
		return *this;
	}

}; // End class point_base<T,D,INTERNAL>


template<typename T, unsigned int D, typename INTERNAL>
class container<point_base<T,D,INTERNAL>> {
// Typedefs
protected:

	typedef point_base<T,D,INTERNAL> value_t;
	typedef INTERNAL                 internal_t;


// Variables
private:

	const value_t& m_obj;


// Constructor
public:

	container( const value_t& obj ) : m_obj( obj ) { }


// Methods
public:

	internal_t operator [] ( unsigned int i ) const {
		printf( "=== point_base: %u ===\n", i );
		return m_obj[i];
	}

}; // End class container<point_base<T,D,INTERNAL>>


template<typename T, unsigned int D, typename INTERNAL = float>
class point : public point_base<T,D,INTERNAL> {
// Typedefs
protected:

	typedef point_base<T,D,INTERNAL> base_t;
	typedef INTERNAL                 internal_t;


// Constructors
public:

	point( ) : base_t( ) { }
	point( const base_t& pt ) : base_t( pt ) { }
	point( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename Expr>
	point( const Expr& expr ) : base_t( expr ) { }
	template<typename ... Args>
	point( T value, Args... values ) : base_t( value, values... ) { }


// Methods
public:

	T dot( const point<T,D>& pt ) {
		T sum = 0;
		for( unsigned int i = 0; i < D; ++i ) {
			sum += base_t::m_data[i] * pt.m_data[i];
		}
		return sum;
	}

}; // End class point<T,D>


template<typename T, typename INTERNAL>
class point<T,2,INTERNAL> : public point_base<T,2,INTERNAL> {
// Typedefs
protected:

	typedef point_base<T,2,INTERNAL> base_t;
	typedef INTERNAL                 internal_t;


// Constructors
public:

	point( ) : base_t( ) { }
	point( const base_t& pt ) : base_t( pt ) { }
	point( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename Expr>
	point( const Expr& expr ) : base_t( expr ) { }
	point( T x ) : base_t( x ) { }
	point( T x, T y ) : base_t( x, y ) { }


// Methods
public:

	T&  x( ) { return base_t::m_data[0]; }
	T&  y( ) { return base_t::m_data[1]; }
	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }

	T dot( const point<T,2>& pt ) {
		return base_t::m_data[0] * pt.m_data[0] + base_t::m_data[1] * pt.m_data[1];
	}

	T cross( const point<T,2>& pt ) const {
		return base_t::m_data[0] * pt.m_data[1] - base_t::m_data[1] * pt.m_data[0];
	}

	const T* to_gl( ) const { return base_t::m_data.data( ); }

}; // End class point<T,2>


template<typename T, typename INTERNAL>
class point<T,3,INTERNAL> : public point_base<T,3,INTERNAL> {
// Typedefs
protected:

	typedef point_base<T,3,INTERNAL> base_t;
	typedef INTERNAL                 internal_t;


// Constructors
public:

	point( ) : base_t( ) { }
	point( const base_t& pt ) : base_t( pt ) { }
	point( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename Expr>
	point( const Expr& expr ) : base_t( expr ) { }
	point( T x ) : base_t( x ) { }
	point( T x, T y ) : base_t( x, y ) { }
	point( T x, T y, T z ) : base_t( x, y, z ) { }


// Methods
public:

	T&  x( ) { return base_t::m_data[0]; }
	T&  y( ) { return base_t::m_data[1]; }
	T&  z( ) { return base_t::m_data[2]; }
	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }
	T   z( ) const { return base_t::m_data[2]; }

	T dot( const point<T,3>& pt ) {
		return base_t::m_data[0] * pt.m_data[0] +
		       base_t::m_data[1] * pt.m_data[1] +
		       base_t::m_data[2] * pt.m_data[2];
	}

	point<T,3> cross( const point<T,3>& pt ) const {
		point<T,3> result;
		result.m_data[0] = base_t::m_data[1] * pt.m_data[2] -
		                   base_t::m_data[2] * pt.m_data[1];
		result.m_data[1] = base_t::m_data[0] * pt.m_data[2] -
		                   base_t::m_data[2] * pt.m_data[0];
		result.m_data[2] = base_t::m_data[0] * pt.m_data[1] -
		                   base_t::m_data[1] * pt.m_data[0];
		return result;
	}

	const T* to_gl( ) const { return base_t::m_data.data( ); }

}; // End class point<T,3>


template<typename T, typename INTERNAL>
class point<T,4,INTERNAL> : public point_base<T,4,INTERNAL> {
// Typedefs
protected:

	typedef point_base<T,4,INTERNAL> base_t;
	typedef INTERNAL                 internal_t;


// Constructors
public:

	point( ) : base_t( ) { }
	point( const base_t& pt ) : base_t( pt ) { }
	point( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename Expr>
	point( const Expr& expr ) : base_t( expr ) { }
	point( T x ) : base_t( x ) { }
	point( T x, T y ) : base_t( x, y ) { }
	point( T x, T y, T z ) : base_t( x, y, z ) { }
	point( T x, T y, T z, T w ) : base_t( x, y, z, w ) { }


// Methods
public:

	T&  x( ) { return base_t::m_data[0]; }
	T&  y( ) { return base_t::m_data[1]; }
	T&  z( ) { return base_t::m_data[2]; }
	T&  w( ) { return base_t::m_data[3]; }
	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }
	T   z( ) const { return base_t::m_data[2]; }
	T   w( ) const { return base_t::m_data[3]; }

	T dot( const point<T,4>& pt ) {
		return base_t::m_data[0] * pt.m_data[0] +
		       base_t::m_data[1] * pt.m_data[1] +
		       base_t::m_data[2] * pt.m_data[2] +
		       base_t::m_data[3] * pt.m_data[3];
	}

	const T* to_gl( ) const { return base_t::m_data.data( ); }

}; // End class point<T,4>


// Various typedefs to make usage easier
typedef point<int,2>           point2i;//
typedef point<float,2>         point2f;
typedef point<double,2>        point2d;
typedef point<unsigned int,2>  point2u;//

typedef point<int,3>           point3i;
typedef point<float,3>         point3f;
typedef point<double,3>        point3d;
typedef point<unsigned int,3>  point3u;

typedef point<int,4>           point4i;
typedef point<float,4>         point4f;
typedef point<double,4>        point4d;
typedef point<unsigned int,4>  point4u;


}  // End namespace euclib

#endif // EUBLIB_POINT_HPP

