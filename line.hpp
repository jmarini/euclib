/*	line.hpp  v 0.1.3.10.1117
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

// TODO: this is segment, make another line class?
//       could need significant changes

namespace euclib {

template<typename T>
class line2 {
// Typdefs
protected:

	typedef std::numeric_limits<T> limit_t;

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
		if( less_equal( p1.x, p2.x ) ) {
			pt1 = p1;
			pt2 = p2;
		}
		else {
			pt1 = p2;
			pt2 = p1;
		}
		check_valid( );
	}
	line2( T x1, T y1, T x2, T y2 ) {
		if( less_equal( x1, x2) ) {
			pt1 = point2<T>{ x1, y1 };
			pt2 = point2<T>{ x2, y2 };
		}
		else {
			pt1 = point2<T>{ x2, y2 };
			pt2 = point2<T>{ x1, y1 };
		}
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

	T width( )  const { return std::abs( pt1.x - pt2.x ); }
	T height( ) const { return std::abs( pt1.y - pt2.y ); }
	
	// TODO: I don't like forcing these to float...

	float slope( ) const {
		if( std::abs( pt1.x - pt2.x ) <= limit_t::epsilon( ) ) { // same x coordinate
			return std::numeric_limits<float>::infinity( );
		}
		
		return (float)( (pt2.y - pt1.y) / (pt2.x - pt1.x) );		
	}
	
	float length( ) const {
		return sqrt( (float)( width( )*width( ) + height( )*height( ) ) );
	}
	
	float intercept( ) const {
		if( slope( ) == std::numeric_limits<float>::infinity( ) ) {
			return std::numeric_limits<float>::infinity( );
		}
		return (float)( pt1.y - slope( ) * pt1.x );
	}


	// TODO: What to do about the inter/extrapolate functions
	//       they are cluttered, an may not have a use in a segment class

	// Extrapolate beyond the bounds of the line segment.
	// Distance: the length beyond the line to move, sign determines direction.
	point2<T> extrapolate( float distance ) {
		T delX = sqrt( ( distance * distance ) / ( 1 + slope( ) * slope( ) ) );
		T delY = slope( ) * delX;
		if ( less_than( distance, 0.f ) ) {
			return point2<T>{ pt1.x - delX, pt1.y - delY };
		}
		else {
			return point2<T>{ pt2.x + delX, pt2.y + delY };
		}
	}

	// value: value(on axis) beyond the line to move, sign determines direction.
	point2<T> extrapolateX( T value ) {
		if ( equal( value, 0 ) ) { return pt1; }
		T delX = value;
		T delY = slope( ) * delX;
		if ( less_than( value, 0 ) ) {
			return point2<T>{ pt1.x + delX, pt1.y + delY };
		}
		else {
			return point2<T>{ pt2.x + delX, pt2.y + delY };
		}
	}
	
	point2<T> extrapolateY( T value ) {
		if ( equal( value, 0 ) ) { return pt1; }
		T delY = value;
		T delX = delY / slope( );
		if ( less_than( value, 0 ) ) {
			return point2<T>{ pt1.x + delX, pt1.y + delY };		
		}
		else { // value < 0
			return point2<T>{ pt2.x + delX, pt2.y + delY };
		}
	}

	// Interpolate between the bounds of the line segment.
	// Distance: the length along the line to move, sign determines direction.
	point2<T> interpolate( float distance ) {
		if ( ( greater_than( distance, 0.f ) ? distance : -distance ) >= length( ) ) {
			return ( distance > 0 ? pt2 : pt1 );
		}
		T delX = sqrt( ( distance * distance ) / ( 1 + slope( ) * slope( ) ) );
		T delY = slope( ) * delX;
		if ( distance < 0 ) {
			return point2<T>( pt1.x + delX, pt1.y + delY );
		}
		else {
			return point2<T>( pt2.x - delX, pt2.y - delY );
		} 
	}

	point2<T> interpolateX( T value ) {
		if ( ( value > 0 ? value : -value ) >= width( ) ) {
			return ( value > 0 ? pt2 : pt1 );
		}
		T delX = value;
		T delY = slope( ) * delX;
		if ( value < 0 ) {
			return point2<T>( pt1.x - delX, pt1.y - delY );
		}
		else {
			return point2<T>( pt2.x - delX, pt2.y - delY );
		}
	}

	point2<T> interpolateY( T value ) {
		if ( ( value > 0 ? value : -value ) >= height( ) ) {
			return ( value > 0 ? pt2 : pt1 );
		}
		T delY = value;
		T delX = delY / slope( );
		if ( value < 0 ) {
			return point2<T>( pt1.x - delX, pt1.y - delY );
		}
		else {
			return point2<T>( pt2.x - delX, pt2.y - delY );
		}
	}

	void print( std::ostream& stream, bool newline = false ) const {
		pt1.print( stream, false );
		stream << "->";
		pt2.print( stream, false);
		if( newline ) { stream << "\n"; }
	}
	
	void gnuplot( std::ostream& stream ) const {
		pt1.gnuplot( stream );
		pt2.gnuplot( stream );
		stream << "e\n";
	}

private:

	void check_valid( ) {
		if( pt1 == point2<T>::null( ) || pt2 == point2<T>::null( ) ) {
			set_null( );
		}
		else if( pt1 == pt2 ) {
			set_null( );
		}
	}
	
	void set_null( ) {
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
		pt1 = line.pt1;
		pt2 = line.pt2;
		line.pt1 = line.pt2 = { }; // line will be deleted after this call
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

