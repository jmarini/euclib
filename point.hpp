/*	point.hpp  v 0.1.5.10.1117
 *
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

#include <ostream>
#include <limits>
#include <complex>

namespace euclib {

	// Helper functions for comparisons, because no assumptions
	//   can be made about the exactness of type T

	template<typename T>
	bool equal( T lhs, T rhs ) {
		return std::abs( lhs - rhs ) <= std::numeric_limits<T>::epsilon( );
	}

	template<typename T>
	bool not_equal( T lhs, T rhs ) {
		return !(lhs == rhs);
	}

	template<typename T>
	bool less_than( T lhs, T rhs ) {
		return rhs - lhs > std::numeric_limits<T>::epsilon( );
	}

	template<typename T>
	bool less_equal( T lhs, T rhs ) {
		return (lhs < rhs || lhs == rhs);
	}

	template<typename T>
	bool greater_than( T lhs, T rhs ) {
		return lhs - rhs > std::numeric_limits<T>::epsilon( );
	}

	template<typename T>
	bool greater_equal( T lhs, T rhs ) {
		return (lhs > rhs || lhs == rhs);
	}

template<typename T>
class point2 {
// Tyepdefs
protected:

	typedef std::numeric_limits<T> limit_t;

	// This class can only be used with scalar types
	//   or types with a specific specialization
	static_assert( limit_t::is_specialized,
	               "type not compatible with std::numeric_limits" );

// Variables
public:

	T x, y;
	
private:

	static T invalid; // holds either limit_t::infinity or limit_t::max


// Constructors
public:

	point2( ) { set_null( ); }
	point2( const point2<T>& pt ) { *this = pt; }
	point2( point2<T>&& pt ) { *this = std::move( pt ); }
	point2( T _x, T _y ) : x( _x ), y( _y ) { check_valid( ); }


// Methods
public:

	// Returns a null point, defined as ( inf, inf ) or ( max, max )
	//   depending on what type T is.
	static point2<T> null( ) {
		static point2<T> null = point2<T>{ invalid, invalid };
		return null;
	}

	// TODO: maybe make this a little easier to use
	//       i.e. have version that returns string or something
	void print( std::ostream& stream, bool newline = false ) const {
		stream << "[" << x << ", " << y << "]";
		if( newline ) { stream << "\n"; }
	}
	
	// TODO: debugging only
	void gnuplot( std::ostream& stream ) const {
		stream << x << " " << y << "\n";
	}

private:

	void check_valid( ) {
		if( x == invalid || y == invalid ) {
			set_null( );
		}
	}
	
	void set_null( ) {
		x = y = invalid;
	}


// Operators
public:

	point2<T>& operator = ( const point2<T>& pt ) {
		x = pt.x;
		y = pt.y;
		check_valid( );
		return *this;
	}

	// Move assignment, takes advantage of rvalues
	point2<T>& operator = ( point2<T>&& pt ) {
		x = pt.x;
		y = pt.y;
		pt.x = pt.y = 0; // pt will be deleted after this call
		check_valid( );
		return *this;
	}

	bool operator == ( const point2<T>& pt ) const {
		if( equal( pt.x, x ) &&
		    equal( pt.y, y )
		  ) {
		  return true;
		}

		// checking for null equality
		if( ( pt.x == invalid || pt.y == invalid ) &&
		    (    x == invalid ||    y == invalid )
		  ) {
			return true;
		}
		
		return false;
	}
	
	bool operator != ( const point2<T>& pt ) const {
		return !(*this == pt);
	}
	
	
}; // End class point2<T>

typedef point2<int>           point2i;
typedef point2<float>         point2f;
typedef point2<unsigned int>  point2u;

// Initialize invalid with either infinity or max
template<typename T>
T point2<T>::invalid = ( point2<T>::limit_t::has_infinity ?
                             point2<T>::limit_t::infinity( )
                         : // else
                             point2<T>::limit_t::max( )
                       );

}  // End namespace euclib

#endif // EUBLIB_POINT_HPP

