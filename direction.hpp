/*	direction.hpp  v 0.1.0.10.1221
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

#ifndef EUBLIB_DIRECTION_HPP
#define EUBLIB_DIRECTION_HPP

#include "euclib_math.hpp"
#include "point.hpp"

namespace euclib {

template<typename T, unsigned int D>
class direction : point_base<T,D> {
// Typedefs
protected:

	typedef point_base<T,D>             base_t;
	typedef std::numeric_limits<double> double_limit_t;


// Variables
private:
	double m_length;
// Constructors
public:

	direction( ) : base_t( ) { m_length = double_limit_t::infinity( ); }
	point( const base_t& pt ) : base_t( pt ) { calc_length( ); }
	point( base_t&& pt ) : base_t( std::forward( pt ) ) { calc_length( ); }
	template<typename ... Args>
	point( T value, Args... values ) : base_t( value, values... ) {
		calc_length( );
	}


// Methods
public:

	direction<double,D> normalize( ) const {
		direction<double,D> result;

		for( unsigned int i = 0; i < D; ++i ) {
			result.set( static_cast<double>(base::m_data[i]) / m_length ); 
		}
		return result;
	}

	double length( ) const { return m_length; }


private:

	void calc_length( ) {
		T length = 0;
		for( unsigned int i = 0; i < D; ++i ) {
			length += result[i] * result[i];
		}
		m_length = sqrt( static_cast<double>(length) );
	}

}; // End class direction<T,D>


// Various typedefs to make usage easier
typedef direction<int,2>           direction2i;
typedef direction<float,2>         direction2f;
typedef direction<double,2>        direction2d;
typedef direction<unsigned int,2>  direction2u;

typedef direction<int,3>           direction3i;
typedef direction<float,3>         direction3f;
typedef direction<double,3>        direction3d;
typedef direction<unsigned int,3>  direction3u;

typedef direction<int,4>           direction4i;
typedef direction<float,4>         direction4f;
typedef direction<double,4>        direction4d;
typedef direction<unsigned int,4>  direction4u;

}  // End namespace euclib

#endif // EUCLIB_DIRECTION_HPP

