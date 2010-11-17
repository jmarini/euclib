/*	point.hpp  v 0.1.3.10.1117
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

namespace euclib {

template<typename T>
class point2 {
// Tyepdefs
protected:

	typedef std::numeric_limits<T> lim;

	// This class can only be used with numeric types
	static_assert( lim::is_specialized,
	               "type not compatible with std::numeric_limits" );

// Variables
public:

	T x, y;


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
		if( lim::has_infinity ) {
			return point2<T>( lim::infinity( ), lim::infinity( ) );
		}
		else {
			return point2<T>( lim::max( ), lim::max( ) );
		}
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
		if( lim::has_infinity &&
		    ( x == lim::infinity( ) || y == lim::infinity( ) )
		  ) {
			set_null( );
		}
		else if( x == lim::max( ) || y == lim::max( ) ) {
			set_null( );
		}
	}
	
	void set_null( ) {
		if( lim::has_infinity ) {
			x = y = lim::infinity( );
		}
		else {
			x = y = lim::max( );
		}
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
		if( pt.x == x && pt.y == y ) { return true; }

		// checking for null equality
		// a point counts as null if x or y has the value infinity/max
		if( lim::has_infinity &&
		    ( pt.x == lim::infinity( ) || pt.y == lim::infinity( ) ) &&
		    ( x == lim::infinity( ) || y == lim::infinity( ) )
		  ) {
			return true;
		}
		else if( ( pt.x == lim::max( ) || pt.y == lim::max( ) ) &&
		         ( x == lim::max( ) || y == lim::max( ) )
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

}  // End namespace euclib

#endif // EUBLIB_POINT_HPP

