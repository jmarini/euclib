/*	line.hpp  v 0.4.0.10.1222
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

#include "euclib_math.hpp"
#include "point.hpp"
#include "direction.hpp"

namespace euclib {

template<typename T, unsigned int D>
class line_base {
// Typdefs
protected:

	typedef std::numeric_limits<T>      limit_t;
	typedef std::numeric_limits<double> double_limit_t;

	// This class can only be used with scalar types
	//   or types with a specific specialization
	static_assert( limit_t::is_specialized,
	               "type not compatible with std::numeric_limits" );

// Variables
protected:

	point<T,D>          m_point;      // line defined as passing through a point
	direction<double,D> m_direction;  //   in a direction

	static T invalid; // holds either limit_t::infinity or limit_t::max


// Constructors
public:

	line_base( ) { set_null( ); }
	line_base( const line_base<T,D>& line ) { *this = line; }
	line_base( line_base<T,D>&& line ) { *this = std::move( line ); }
	line_base( const point<T,D>& pt1, const point<T,D>& pt2 ) {
		m_point = pt1;
		m_direction = direction<double,D> {
		                  static_cast<double>( pt2.x( ) - pt1.x( ) ),
		                  static_cast<double>( pt2.y( ) - pt1.y( ) )
		              };
		check_valid( );
	}
	template<typename R>
	line_base( const point<T,D>& pt, const direction<R,D>& dir ) :
		m_point( pt ) {
		for( unsigned int i = 0; i < D; ++i ) {
			m_direction = static_cast<double>( dir[i] );
		}
		m_direction.calc_length( );
		check_valid( );
	}


// Methods
public:

	// Returns a null line, defined as point2<T>::null --> point2<T>::null
	static line_base<T,D> null( ) {
		static line_base<T,D> null = line_base<T,D>{
		                                 point<T,D>::null( ),
		                                 direction<double,D>::null( )
		                             };
		return null;
	}

	point<T,D>          base_point( ) const     { return m_point; }
	direction<double,D> base_direction( ) const { return m_direction; }


protected:

	inline void check_valid( ) {
		if( m_point == point<T,D>::null( ) ||
		    m_direction == direction<double,D>::null( )
		  ) {
			set_null( );
		}
	}

	inline void set_null( ) {
		m_point = point<T,D>::null( );
		m_direction = direction<double,D>::null( );
	}


// Operators
public:

	line_base<T,D>& operator = ( const line_base<T,D>& line ) {
		m_point = line.m_point;
		m_direction = line.m_direction;
		check_valid( );
		return *this;
	}

	line_base<T,D>& operator = ( line_base<T,D>&& line ) {
		std::swap( m_point, line.m_point );
		std::swap( m_direction, line.m_direction );
		check_valid( );
		return *this;
	}

	// TODO: needs to be fixed, different for line and segment
	bool operator == ( const line_base<T,D>& line ) const {
		return ( m_point == line.m_point && m_direction == line.m_direction );
	}

	bool operator != ( const line_base<T,D>& line ) const {
		return !(*this == line);
	}

}; // End class line_base<T,D>

template<typename T, unsigned int D>
class line : public line_base<T,D> {
// Typedefs
protected:

	typedef line_base<T,D>              base_t;
	typedef std::numeric_limits<T>      limit_t;
	typedef std::numeric_limits<double> double_limit_t;


// Constructors
public:

	line( ) : base_t( ) { }
	line( const base_t& line ) : base_t( line ) { }
	line( base_t&& line ) : base_t( std::forward<base_t>( line ) ) { }
	line( const point<T,D>& pt1, const point<T,D>& pt2 ) : base_t( pt1, pt2 ) { }
	template<typename R>
	line( const point<T,D>& pt, const direction<R,D>& dir ) : base_t( pt, dir ) { }

}; // End class line<T,D>


template<typename T>
class line<T,2> : public line_base<T,2> {
// Typedefs
protected:

	typedef line_base<T,2>              base_t;
	typedef std::numeric_limits<T>      limit_t;
	typedef std::numeric_limits<double> double_limit_t;


// Constructors
public:

	line( ) : base_t( ) { }
	line( const base_t& line ) : base_t( line ) { }
	line( base_t&& line ) : base_t( std::forward<base_t>( line ) ) { }
	line( const point<T,2>& pt1, const point<T,2>& pt2 ) : base_t( pt1, pt2 ) { }
	template<typename R>
	line( const point<T,2>& pt, const direction<R,2>& dir ) : base_t( pt, dir ) { }


// Methods
public:

	double slope( ) const {
		// vertical line
		if( equal( base_t::m_direction.x( ), 0 ) ) {
			return double_limit_t::infinity( );
		}

		return base_t::m_direction.y( ) / base_t::m_direction.x( );
	}

	T intercept( ) const {
		// vertical line
		if( equal( base_t::m_direction.x( ), 0 ) ) {
			return double_limit_t::infinity( );
		}

		double tmp = slope( ) * static_cast<double>(base_t::m_point.x( ));
		return base_t::m_point.y( ) - static_cast<T>( round_nearest<T>(tmp) );
	}

	T at_x( T x ) const {
		// vertical line
		if( equal( base_t::m_direction.x( ), 0 ) ) {
			return base_t::invalid;
		}
		// horizontal line
		else if( equal( base_t::m_direction.y( ), 0 ) ) {
			return base_t::m_point.y( );
		}

		float tmp = slope( ) * static_cast<double>(x);
		return intercept( ) + static_cast<T>( round_nearest<T>(tmp) );
	}

	T at_y( T y ) const {
		// vertical line
		if( equal( base_t::m_direction.x( ), 0 ) ) {
			return base_t::m_point.x( );
		}
		// horizontal line
		else if( equal( base_t::m_direction.y( ), 0 ) ) {
			return base_t::invalid;
		}

		double tmp = static_cast<double>(base_t::m_point.x( )) - intercept( );
		tmp *= base_t::m_direction.x( ) / base_t::m_direction.y( ); // tmp * 1/slope
		return static_cast<T>( round_nearest<T>(tmp) );
	}

}; // End class line<T,2>


// Various typedefs to make usage easier
typedef line<int,2>           line2i;
typedef line<float,2>         line2f;
typedef line<double,2>        line2d;
typedef line<unsigned int,2>  line2u;

// Initialize invalid with either infinity or max
template<typename T, unsigned int D>
T line_base<T,D>::invalid = ( line<T,D>::limit_t::has_infinity ?
                                  line<T,D>::limit_t::infinity( )
                              : // else
                                  line<T,D>::limit_t::max( )
                            );

}  // End namespace euclib

#endif // EUBLIB_LINE_HPP

