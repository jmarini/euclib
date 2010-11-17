/*	rect.hpp  v 0.1.3.10.1117
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

#ifndef EUBLIB_RECT_HPP
#define EUBLIB_RECT_HPP

#include <ostream>
#include <limits>
#include "line.hpp"
#include "point.hpp"

namespace euclib {

template<typename T>
class rect2 {
// Typedefs
protected:

	typedef std::numeric_limits<T> limit_t;

	// This class can only be used with scalar types
	//   or types with a specific specialization
	static_assert( limit_t::is_specialized,
	               "type not compatible with std::numeric_limits" );

// Variables
public:

	T l, r, t, b;

private:

	static T invalid; // holds either limit_t::infinity or limit_t::max


// Constructors
public:

	rect2( ) { set_null( ); }
	rect2( const rect2<T>& rect ) { *this = rect; }
	rect2( rect2<T>&& rect ) { *this = std::move( rect ); }
	rect2( T left, T right, T top, T bottom ) :
		l( left ),
		r( right ),
		t( top ),
		b( bottom ) {
		check_valid( );
	}
	rect2( const point2<T>& location, T width, T height ) :
		l( location.x ),
		r( location.x + width ),
		t( location.y ),
		b( location.y + height ) {
		check_valid( );
	}


// Methods
public:

	// Returns a null rect, defined as inf/max for all
	static rect2<T> null( ) {
		static rect2<T> null = rect2<T>{ invalid, invalid, invalid, invalid };
		return null;
	}

	T  width( )  const { return r - l; }
	T  height( ) const { return b - t; }
	
	point2<T> tl( ) const { return point2<T>( l, t ); }
	point2<T> tr( ) const { return point2<T>( r, t ); }
	point2<T> br( ) const { return point2<T>( r, b ); }
	point2<T> bl( ) const { return point2<T>( l, b ); }
	
	line2<T> left( )   const { return line2<T>( tl( ), bl( ) ); }
	line2<T> right( )  const { return line2<T>( tr( ), br( ) ); }
	line2<T> top( )    const { return line2<T>( tl( ), tr( ) ); }
	line2<T> bottom( ) const { return line2<T>( bl( ), br( ) ); }
	
	T area( )      const { return width( ) * height( ); }
	T perimeter( ) const { return 2 * width( ) + 2 * height( ); }

private:

	void check_valid( ) {
		// check null
		if( l == invalid || r == invalid ||
		    t == invalid || b == invalid
		  ) {
			set_null( );
		}
		// is l > r or t > b
		else if( l - r >= limit_t::epsilon( ) || t - b >= limit_t::epsilon( ) ) {
			set_null( );
		}
	}
	
	void set_null( ) {
		l = r = t = b = invalid;
	}

// Operators
public:

	rect2<T>& operator = ( const rect2<T>& rect ) {
		l = rect.l;
		r = rect.r;
		t = rect.t;
		b = rect.b;
		check_valid( );
		return *this;
	}
	
	rect2<T>& operator = ( rect2<T>&& rect ) {
		l = rect.l;
		r = rect.r;
		t = rect.t;
		b = rect.b;
		rect.l = rect.r = rect.t = rect.b = 0; // rect will be deleted
		check_valid( );
		return *this;
	}

	bool operator == ( const rect2<T>& rect ) const {
		if( l == rect.l && r == rect.r && t == rect.t && b == rect.b ) {
			return true;
		}
		
		// checking for null equality
		// a rect is null if any value is inf/max or if l > r or t > b
		if( ( l == invalid || r == invalid ||
		      t == invalid || b == invalid ) &&
		    ( rect.l == invalid || rect.r == invalid ||
		      rect.t == invalid || rect.b == invalid )
		   ) {
		   	return true;
		}
		// check l > r or t < b
		else if( ( l - r >= limit_t::epsilon( ) ||
		           t - b >= limit_t::epsilon( ) ) &&
		         ( rect.l - rect.r >= limit_t::epsilon( ) ||
		           rect.t - rect.b >= limit_t::epsilon( ) )
		       ) {
			return true;
		}
		return false;
	}
	
	bool operator != ( const rect2<T>& rect ) const {
		return !(*this == rect);
	}
	
	friend std::ostream& operator << ( std::ostream& stream, const rect2<T>& rect ) {
		#ifdef GNUPLOT
			return stream << rect.l << " " << rect.t << "\n"
			              << rect.r << " " << rect.t << "\n"
			              << rect.r << " " << rect.b << "\n"
			              << rect.l << " " << rect.b << "\n"
			              << rect.l << " " << rect.t << "\n"
			              << "e\n";
		#else
			return stream << rect.l << " " << rect.r << " "
			              << rect.t << " " << rect.b;
		#endif
	}


}; // End class rect2<T>

// useful typedefs
typedef rect2<int>           rect2i;
typedef rect2<float>         rect2f;
typedef rect2<unsigned int>  rect2u;

// Initialize invalid with either infinity or max
template<typename T>
T rect2<T>::invalid = ( rect2<T>::limit_t::has_infinity ?
                            rect2<T>::limit_t::infinity( )
                        : // else
                            rect2<T>::limit_t::max( )
                      );

}  // End namespace euclib

#endif // EUBLIB_RECT_HPP

