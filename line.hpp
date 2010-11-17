/*	line.hpp  v 0.1.4.10.1117
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

#ifndef EUBLIB_LINE_HPP
#define EUBLIB_LINE_HPP

#include <ostream>
#include <limits>
#include <complex>
#include "point.hpp"
#include "segment.hpp"

namespace euclib {

template<typename T>
class line2 {
// Typdefs
protected:

	typedef std::numeric_limits<T>     limit_t;
	typedef std::numeric_limits<float> float_limit_t;

	// This class can only be used with scalar types
	//   or types with a specific specialization
	static_assert( limit_t::is_specialized,
	               "type not compatible with std::numeric_limits" );

// Variables
public:

	point2<T> pt1;
	point2<T> pt2;

private:

	static T invalid; // holds either limit_t::infinity or limit_t::max


// Constructors
public:

	line2( ) { set_null( ); }
	line2( const line2<T>& line ) { *this = line; }
	line2( line2<T>&& line ) { *this = std::move( line ); }
	line2( const point2<T>& p1, const point2<T>& p2 ) {
		pt1 = p1;
		pt2 = p2;
		check_valid( );
	}
	line2( T x1, T y1, T x2, T y2 ) {
		pt1 = point2<T>{ x1, y1 };
		pt2 = point2<T>{ x2, y2 };
		check_valid( );
	}
	line2( const segment2<T>& segment ) {
		pt1 = segment.pt1;
		pt2 = segment.pt2;
		check_valid( );
	}


// Methods
public:

	// Returns a null line, defined as point2<T>::null --> point2<T>::null
	static line2<T> null( ) {
		static line2<T> null = line2<T>{ point2<T>::null( ),
		                                 point2<T>::null( ) };
		return null;
	}

	float slope( ) const {
		// same x coordinate
		if( equal(pt1.x, pt2.x) ) {
			return float_limit_t::infinity( );
		}
		
		return static_cast<float>(pt2.y-pt1.y) / static_cast<float>(pt2.x-pt1.x);
	}
	
	T intercept( ) const {
		if( slope( ) == float_limit_t::infinity( ) ) {
			return float_limit_t::infinity( );
		}

		float tmp = slope( ) * static_cast<float>(pt1.x);
		if( limit_t::is_integer ) { tmp += 0.5f; } // reduce rounding errors

		return pt1.y - static_cast<T>(tmp);
	}

	T at_x( T x ) const {
		float slp = slope( );
		// vertical line
		if( slp == float_limit_t::infinity( ) ) {
			return invalid;
		}
		// horizontal line
		else if( equal( slp, 0.f ) ) {
			return pt1.y;
		}

		float tmp = slp * static_cast<float>(x);
		if( limit_t::is_integer ) { tmp += 0.5f; } // reduce rounding errors
		return intercept( ) + static_cast<T>(tmp);
	}
	
	T at_y( T y ) const {
		float slp = slope( );
		// vertical line
		if( slp == float_limit_t::infinity( ) ) {
			return pt1.x;
		}
		// horizontal line
		else if( equal( slp, 0.f ) ) {
			return invalid;
		}

		float tmp = static_cast<float>( pt1.x - intercept( ) );
		if( limit_t::is_integer ) { tmp += 0.5f; } // reduce rounding errors
		return static_cast<T>( tmp / slp );
	}


private:

	inline void check_valid( ) {
		if( pt1 == point2<T>::null( ) || pt2 == point2<T>::null( ) ) {
			set_null( );
		}
		else if( pt1 == pt2 ) {
			set_null( );
		}
	}
	
	inline void set_null( ) {
		pt1 = pt2 = point2<T>::null( );
	}


// Operators
public:

	line2<T>& operator = ( const line2<T>& line ) {
		pt1 = line.pt1;
		pt2 = line.pt2;
		check_valid( );
		return *this;
	}
	
	line2<T>& operator = ( line2<T>&& line ) {
		std::swap( pt1, line.pt1 );
		std::swap( pt2, line.pt2 );
		check_valid( );
		return *this;
	}
	
	// Checks either orientation
	bool operator == ( const line2<T>& line ) const {
		return ( pt1 == line.pt1 && pt2 == line.pt2 ) ||
		       ( pt1 == line.pt2 && pt2 == line.pt1 );
	}
	
	bool operator != ( const line2<T>& line ) const {
		return !(*this == line);
	}
	
	// TODO: gnuplot cannot handle inf in equation
	//       needs some work to properly handle everything
	friend std::ostream& operator << ( std::ostream& stream,
	                                   const line2<T>& line ) {
		#ifdef GNUPLOT
			if( line.slope( ) == float_limit_t::infinity( ) ) {
				return stream << "(y) = " << line.pt1.x << "\n";
			}
			else {
				return stream << "(x) = " << line.slope( )
				              << " * x + " << line.intercept( ) << "\n";
			}
		#else
			return stream << line.pt1 << "-->" << line.pt2;
		#endif
	}


}; // End class line2<T>

typedef line2<int>           line2i;
typedef line2<float>         line2f;
typedef line2<unsigned int>  line2u;

// Initialize invalid with either infinity or max
template<typename T>
T line2<T>::invalid = ( line2<T>::limit_t::has_infinity ?
                            line2<T>::limit_t::infinity( )
                        : // else
                            line2<T>::limit_t::max( )
                      );

}  // End namespace euclib

#endif // EUBLIB_LINE_HPP

