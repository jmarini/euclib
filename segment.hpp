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

/*
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
*/

typedef segment<float,2>         segment2f;
typedef segment<double,2>        segment2d;

}  // End namespace euclib

#endif // EUBLIB_SEGMENT_HPP
