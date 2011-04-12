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

#ifndef EUBLIB_SEGMENT_HPP
#define EUBLIB_SEGMENT_HPP

#include "euclib_math.hpp"
#include "line.hpp"

namespace euclib {

template<typename T, unsigned int D>
class segment : public line_base<T,D> {
// Typedefs
protected:

	typedef line_base<T,D>         base_t;
	typedef std::numeric_limits<T> limit_t;


// Constructors
public:

	segment( ) : base_t( ) { }
	segment( const base_t& line ) : base_t( line ) { }
	segment( base_t&& line ) : base_t( std::forward<base_t>( line ) ) { }
	segment( const point<T,D>& pt1, const point<T,D>& pt2 ) : base_t( pt1, pt2 ) { }
	segment( const point<T,D>& pt, const vector<T,D>& vec ) : base_t( pt, vec ) { }


// Methods
public:

	inline T length( ) const { return base_t::m_vector.length( ); }
	inline T length_sq( ) const { return base_t::m_vector.length_sq( ); }

	// positive starts from end of direction
	// negative starts from base of direction
	point<T,D> extrapolate( T distance ) const {
		T t = distance / base_t::m_vector.length( );
		if( greater_than( distance, 0 ) ) { t += 1; }
		point<T,D> result = base_t::m_point + t * base_t::m_vector;
		return result;
	}

	// positive starts from base of direction
	// negative starts from end of direction
	point<T,D> interpolate( T distance ) const {
		T t = distance / base_t::m_vector.length( );
		if( less_than( distance, 0 ) ) { t += 1; }
		return point<T,D>{ base_t::m_point + t * base_t::m_vector };
	}

}; // End class segment<T,D>


template<typename T>
class segment<T,2> : public line_base<T,2> {
// Typedefs
protected:

	typedef line_base<T,2>         base_t;
	typedef std::numeric_limits<T> limit_t;


// Constructors
public:

	segment( ) : base_t( ) { }
	segment( const base_t& line ) : base_t( line ) { }
	segment( base_t&& line ) : base_t( std::forward<base_t>( line ) ) { }
	segment( const point<T,2>& pt1, const point<T,2>& pt2 ) : base_t( pt1, pt2 ) { }
	segment( const point<T,2>& pt, const vector<T,2>& vec ) : base_t( pt, vec ) { }


// Methods
public:

	inline T length( ) const { return base_t::m_vector.length( ); }
	inline T length_sq( ) const { return base_t::m_vector.length_sq( ); }

	T width( ) const  { return base_t::m_vector[0]; }
	T height( ) const { return base_t::m_vector[1]; }

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

	bool horizontal( ) const { return equal( base_t::m_vector[1], 0 ); }
	bool vertical( ) const   { return equal( base_t::m_vector[0], 0 ); }

	////////////////////////////////////////////////
	// positive starts from end of direction
	// negative starts from base of direction
	// for all interpolate/extrapolate functions
	point<T,2> extrapolate( T distance ) const {
		T t = distance / base_t::m_vector.length( );
		if( greater_than( distance, 0 ) ) { t += 1; }
		return point<T,2>{ base_t::m_point + t * base_t::m_vector };
	}

	point<T,2> extrapolate_x( T x ) const {
		T t = x / base_t::m_vector[0];
		if( greater_than( x, 0 ) ) { t += 1; }
		return point<T,2>{ base_t::m_point + t * base_t::m_vector };
	}

	point<T,2> extrapolate_y( T y ) const {
		T t = y / base_t::m_vector[1];
		if( greater_than( y, 0 ) ) { t += 1; }
		return point<T,2>{ base_t::m_point + t * base_t::m_vector };
	}

	point<T,2> interpolate( T distance ) const {
		T t = distance / base_t::m_vector.length( );
		if( less_than( distance, 0 ) ) { t += 1; }
		return point<T,2>{ base_t::m_point + t * base_t::m_vector };
	}

	point<T,2> interpolate_x( T x ) const {
		T t = x / base_t::m_vector[0];
		if( less_than( x, 0 ) ) { t += 1; }
		return point<T,2>{ base_t::m_point + t * base_t::m_vector };
	}

	point<T,2> interpolate_y( T y ) const {
		T t = y / base_t::m_vector[1];
		if( less_than( y, 0 ) ) { t += 1; }
		return point<T,2>{ base_t::m_point + t * base_t::m_vector };
	}

}; // End class segment<T,2>


typedef segment<float,2>         segment2f;
typedef segment<double,2>        segment2d;

}  // End namespace euclib

#endif // EUBLIB_SEGMENT_HPP

