/*
 *	Copyright (C) 2010-2011 Jonathan Marini
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef EUBLIB_LINE_HPP
#define EUBLIB_LINE_HPP

#include "type_traits.hpp"
#include "euclib_math.hpp"
#include "point.hpp"
#include "vector.hpp"

namespace euclib {

template<typename T, unsigned int D>
class line_base {
// Typdefs
protected:

	typedef std::numeric_limits<T>          limit_t;

	// This class can only be used with floating point types
	//   soon, support will be added for fixed point types
	static_assert( std::is_floating_point<T>::value || mpl::is_decimal<T>::value,
	               "T must be floating point or decimal" );
	static_assert( D != 0, "cannot have 0-dimensional object" );


// Variables
protected:

	point<T,D>  m_point;   // line defined as passing through a point
	vector<T,D> m_vector;  //   with a direction vector


// Constructors
protected: // cannot construct directly

	line_base( ) { }
	line_base( const line_base<T,D>& line ) { *this = line; }
	line_base( line_base<T,D>&& line ) { *this = std::move( line ); }
	line_base( const point<T,D>& pt1, const point<T,D>& pt2 ) : m_point(pt1), m_vector(pt2 - pt1) { }
	line_base( const point<T,D>& pt, const vector<T,D>& vec ) : m_point( pt ), m_vector( vec ) { }

	virtual ~line_base( ) { }

// Methods
public:

	const point<T,D>&  base_point( ) const  { return m_point; }
	const vector<T,D>& base_vector( ) const { return m_vector; }

	unsigned int dimension( ) const { return D; }


// Operators
public:

	line_base<T,D>& operator = ( const line_base<T,D>& line ) {
		m_point = line.m_point;
		m_vector = line.m_vector;
		return *this;
	}

	line_base<T,D>& operator = ( line_base<T,D>&& line ) {
		std::swap( m_point, line.m_point );
		std::swap( m_vector, line.m_vector );
		return *this;
	}

	// TODO: needs to be fixed, different for line and segment
	bool operator == ( const line_base<T,D>& line ) const {
		T slope1, slope2;
		for( unsigned int i = 1; i < D; ++i ) {
			slope1 = m_vector[i] / m_vector[0];
			slope2 = line.m_vector[i] / line.m_vector[0];
			if( not_equal( slope1, slope2 ) ) { return false; }
			// check intercepts...
		}

		return true;
	}

	bool operator != ( const line_base<T,D>& line ) const {
		return !(*this == line);
	}

}; // End class line_base<T,D>

template<typename T, unsigned int D>
class line : public line_base<T,D> {
// Typedefs
protected:

	typedef line_base<T,D>         base_t;
	typedef std::numeric_limits<T> limit_t;


// Constructors
public:

	line( ) : base_t( ) { }
	line( const base_t& line ) : base_t( line ) { }
	line( base_t&& line ) : base_t( std::forward<base_t>( line ) ) { }
	line( const point<T,D>& pt1, const point<T,D>& pt2 ) : base_t( pt1, pt2 ) { }
	line( const point<T,D>& pt, const vector<T,D>& vec ) : base_t( pt, vec ) { }

}; // End class line<T,D>


template<typename T>
class line<T,2> : public line_base<T,2> {
// Typedefs
protected:

	typedef line_base<T,2>         base_t;
	typedef std::numeric_limits<T> limit_t;


// Constructors
public:

	line( ) : base_t( ) { }
	line( const base_t& line ) : base_t( line ) { }
	line( base_t&& line ) : base_t( std::forward<base_t>( line ) ) { }
	line( const point<T,2>& pt1, const point<T,2>& pt2 ) : base_t( pt1, pt2 ) { }
	line( const point<T,2>& pt, const vector<T,2>& vec ) : base_t( pt, vec ) { }


// Methods
public:

	bool horizontal( ) const { return equal( base_t::m_vector[1], 0 ); }
	bool vertical( ) const   { return equal( base_t::m_vector[0], 0 ); }

	T slope( ) const {
		// vertical line
		if( equal( base_t::m_vector[0], 0 ) ) {
			return limit_t::infinity( );
		}

		return base_t::m_vector[1] / base_t::m_vector[0];
	}

	T inv_slope( ) const {
		// vertical line
		if( equal( base_t::m_vector[0], 0 ) ) {
			return 0;
		}
		// horizontal line
		else if( equal( base_t::m_vector[1], 0 ) ) {
			return limit_t::infinity( );
		}

		return base_t::m_vector[0] / base_t::m_vector[1];
	}

	T intercept( ) const {
		// vertical line
		if( equal( base_t::m_vector[0], 0 ) ) {
			return limit_t::infinity( );
		}

		return base_t::m_point[1] - slope( ) * base_t::m_point[0];
	}

	T at_x( T x ) const {
		// vertical line
		if( equal( base_t::m_vector[0], 0 ) ) {
			return limit_t::infinity( );
		}
		// horizontal line
		else if( equal( base_t::m_vector[1], 0 ) ) {
			return base_t::m_point[1];
		}

		return intercept( ) + slope( ) * x;
	}

	T at_y( T y ) const {
		// vertical line
		if( equal( base_t::m_vector[0], 0 ) ) {
			return base_t::m_point[0];
		}
		// horizontal line
		else if( equal( base_t::m_vector[1], 0 ) ) {
			return limit_t::infinity( );
		}

		T tmp = base_t::m_point[0] - intercept( );
		tmp *= base_t::m_direction[0] / base_t::m_direction[1]; // tmp * 1/slope
		return tmp;
	}

}; // End class line<T,2>


// Various typedefs to make usage easier
typedef line<float,2>         line2f;
typedef line<double,2>        line2d;


}  // End namespace euclib

#endif // EUBLIB_LINE_HPP
