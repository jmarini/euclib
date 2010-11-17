/*	line.hpp  v 0.1.1.10.1117
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

#ifndef EUBLIB_SEGMENT_HPP
#define EUBLIB_SEGMENT_HPP

#include <ostream>
#include <limits>
#include <complex>
#include "point.hpp"

namespace euclib {

template<typename T>
class segment2 {
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

	segment2( ) { set_null( ); }
	segment2( const segment2<T>& segment ) { *this = segment; }
	segment2( segment2<T>&& segment ) { *this = std::move( segment ); }
	segment2( const point2<T>& p1, const point2<T>& p2 ) {
		if( equal( p1.x, p2.x ) ) {
			if( less_than( p1.y, p2.y ) ) {
				pt1 = p1;
				pt2 = p2;
			}
			else {
				pt1 = p2;
				pt2 = p1;
			}
		}
		else {
			if( less_than( p1.x, p2.x ) ) {
				pt1 = p1;
				pt2 = p2;
			}
			else {
				pt1 = p2;
				pt2 = p1;
			}
		}
		check_valid( );
	}
	segment2( T x1, T y1, T x2, T y2 ) {
		*this = segment2{ point2<T>{ x1, y1 }, point2<T>{ x2, y2 } };
	}


// Methods
public:

	// Returns a null segment, defined as point2<T>::null --> point2<T>::null
	static segment2<T> null( ) {
		static segment2<T> null = segment2<T>{ point2<T>::null( ),
		                                       point2<T>::null( ) };
		return null;
	}

	T width( )  const { return std::abs( pt1.x - pt2.x ); }
	T height( ) const { return std::abs( pt1.y - pt2.y ); }

	float slope( ) const {
		// same x coordinate
		if( equal(pt1.x, pt2.x) ) {
			return float_limit_t::infinity( );
		}
		
		return static_cast<float>(pt2.y-pt1.y) / static_cast<float>(pt2.x-pt1.x);
	}

	float length( ) const {
		float tmp = static_cast<float>(width( )*width( ) + height( )*height( ));
		return std::sqrt( tmp );
	}


	// Extrapolate beyond the bounds of the line segment.
	//   distance: the length beyond the segment to move,
	//             sign determines which side of the segment to move from.
	point2<T> extrapolate( float distance ) {
		if( equal( distance, 0.f ) ) { return pt1; }
		
		float slp = slope( );
		// vertical line
		if( slp == float_limit_t::infinity( ) ) {
			if( limit_t::is_integer ) { distance += 0.5f; }
			if ( less_than( distance, 0.f ) ) {
				return point2<T>{ pt1.x, pt1.y + static_cast<T>(distance) };
			}
			else {
				return point2<T>{ pt2.x, pt2.y + static_cast<T>(distance) };
			}
		}

		float del_x = distance*distance / ( 1.f + slp*slp );
		del_x = std::sqrt( del_x );
		float del_y = slp * del_x;

		// reduce rouding errors
		if( limit_t::is_integer ) {
			del_x += 0.5f;
			del_y += 0.5f;
		}

		if( less_than( distance, 0.f ) ) {
			return point2<T>{ pt1.x - static_cast<T>(del_x),
			                  pt1.y - static_cast<T>(del_y) };
		}
		else {
			return point2<T>{ pt2.x + static_cast<T>(del_x),
			                  pt2.y + static_cast<T>(del_y) };
		}
	}

	// value: amount beyond the segment to move,
	//        sign determines which side of the segment to move from.
	point2<T> extrapolateX( T value ) {
		if( equal( value, 0 ) ) { return pt1; }

		float slp = slope( );
		// vertical line
		if( slope( ) == float_limit_t::infinity( ) ) {
			return point2<T>::null( );
		}
		
		T del_x = value;
		float del_y = slope( ) * static_cast<float>(del_x);
		if( limit_t::is_integer ) { del_y += 0.5f; } // reduce rounding errors
		
		if( less_than( value, 0 ) ) {
			return point2<T>{ pt1.x + del_x, pt1.y + static_cast<T>(del_y) };
		}
		else {
			return point2<T>{ pt2.x + del_x, pt2.y + static_cast<T>(del_y) };
		}
	}

	// value: amount beyond the segment to move,
	//        sign determines which side of the segment to move from.
	point2<T> extrapolateY( T value ) {
		if( equal( value, 0 ) ) { return pt1; }

		float slp = slope( );
		// horizontal line
		if( equal( slp, 0.f ) ) { return point2<T>::null( ); }
		else if( slp == float_limit_t::infinity( ) ) {
			if( less_than( value, 0 ) ) {
				return pt1;
			}
			else {
				return pt2;
			}
		}

		T del_y = value;
		float del_x = static_cast<float>(del_y) / slp;
		if( limit_t::is_integer ) { del_y += 0.5f; } // reduce rounding errors

		if( less_than( value, 0 ) ) {
			return point2<T>{ pt1.x + static_cast<T>(del_x), pt1.y + del_y };
		}
		else {
			return point2<T>{ pt2.x + static_cast<T>(del_x), pt2.y + del_y };
		}
	}

	// Interpolate between the bounds of the line segment.
	//   distance: the length along the segment to move,
	//             sign determines which side of the segment to move from.
	point2<T> interpolate( float distance ) {
		if( equal( distance, 0.f ) ) { return pt1; }
		// wanted to interpolate beyond the length of the segment (!)
		else if( greater_equal( std::abs( distance ), length( ) ) ) {
			return ( greater_than( distance, 0.f ) ? pt2 : pt1 );
		}
		
		float slp = slope( );
		// vertical line
		if( slp == float_limit_t::infinity( ) ) {
			if( limit_t::is_integer ) { distance += 0.5f; }
			if( less_than( distance, 0.f ) ) {
				return point2<T>{ pt1.x + static_cast<T>(distance), pt1.y };
			}
			else {
				return point2<T>{ pt2.x + static_cast<T>(distance), pt2.y };
			}
		}

		float del_x = distance*distance / ( 1 + slp*slp );
		del_x = std::sqrt( del_x );
		float del_y = slp * del_x;

		// reduce rouding errors
		if( limit_t::is_integer ) {
			del_x += 0.5f;
			del_y += 0.5f;
		}

		if( less_than( distance, 0.f ) ) {
			return point2<T>{ pt1.x + static_cast<T>(del_x),
			                  pt1.y + static_cast<T>(del_y) };
		}
		else {
			return point2<T>{ pt2.x - static_cast<T>(del_x),
			                  pt2.y - static_cast<T>(del_y) };
		}
	}

	// value: amount in the segment to move,
	//        sign determines which side of the segment to move from.
	point2<T> interpolateX( T value ) {
		if( equal( value, 0 ) ) { return pt1; }
		else if( greater_equal( std::abs(value), width( ) ) ) {
			return ( greater_than( value, 0 ) ? pt2 : pt1 );
		}
		
		float slp = slope( );
		// vertical line
		if( slp == float_limit_t::infinity( ) ) { return point2<T>::null( ); }

		T del_x = value;
		float del_y = slp * static_cast<float>(del_x);
		
		if( limit_t::is_integer ) { del_y += 0.5f; } // reduce rounding errors
		
		if( less_than( value, 0 ) ) {
			return point2<T>( pt1.x - del_x, pt1.y - static_cast<T>(del_y) );
		}
		else {
			return point2<T>( pt2.x - del_x, pt2.y - static_cast<T>(del_y) );
		}
	}

	// value: amount in the segment to move,
	//        sign determines which side of the segment to move from.
	point2<T> interpolateY( T value ) {
		if( equal( value, 0 ) ) { return pt1; }
		else if( greater_equal( std::abs( value ), height( ) ) ) {
			return ( greater( value, 0 ) ? pt2 : pt1 );
		}
		
		float slp = slope( );
		// horizontal line
		if( equal( slp, 0.f ) ) { return point2<T>::null( ); }
		else if( slp == float_limit_t::infinity( ) ) {
			if( less_than( value, 0 ) ) {
				return pt1;
			}
			else {
				return pt2;
			}
		}
		
		T del_y = value;
		float del_x = static_cast<float>(del_y) / slp;
		
		if( limit_t::is_integer ) { del_x += 0.5f; } // reduce rounding errors
		
		if ( less_than( value, 0 ) ) {
			return point2<T>( pt1.x - static_cast<T>(del_x), pt1.y - del_y );
		}
		else {
			return point2<T>( pt2.x - static_cast<T>(del_x), pt2.y - del_y );
		}
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

	segment2<T>& operator = ( const segment2<T>& line ) {
		pt1 = line.pt1;
		pt2 = line.pt2;
		check_valid( );
		return *this;
	}
	
	segment2<T>& operator = ( segment2<T>&& line ) {
		std::swap( pt1, line.pt1 );
		std::swap( pt2, line.pt2 );
		check_valid( );
		return *this;
	}
	
	// Checks either orientation
	bool operator == ( const segment2<T>& line ) const {
		return ( pt1 == line.pt1 && pt2 == line.pt2 ) ||
		       ( pt1 == line.pt2 && pt2 == line.pt1 );
	}
	
	bool operator != ( const segment2<T>& line ) const {
		return !(*this == line);
	}
	
	friend std::ostream& operator << ( std::ostream& stream,
	                                   const segment2<T>& segment ) {
		#ifdef GNUPLOT
			return stream << segment.pt1 << segment.pt2 << "e\n";
		#else
			return stream << segment.pt1 << "-->" << segment.pt2;
		#endif
	}


}; // End class segment2<T>

typedef segment2<int>           segment2i;
typedef segment2<float>         segment2f;
typedef segment2<unsigned int>  segment2u;

// Initialize invalid with either infinity or max
template<typename T>
T segment2<T>::invalid = ( segment2<T>::limit_t::has_infinity ?
                               segment2<T>::limit_t::infinity( )
                           : // else
                               segment2<T>::limit_t::max( )
                         );

}  // End namespace euclib

#endif // EUBLIB_SEGMENT_HPP

