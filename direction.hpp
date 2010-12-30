/*
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

#include <boost/type_traits/is_floating_point.hpp>
#include "euclib_math.hpp"
#include "point.hpp"

namespace euclib {

template<typename T, unsigned int D, typename INTERNAL>
class line_base;

template<typename T, unsigned int D, typename INTERNAL = float>
class direction : public point_base<T,D,INTERNAL> {
// Typedefs
protected:

	typedef point_base<T,D,INTERNAL> base_t;
	typedef INTERNAL                 internal_t;


// Constructors
public:

	direction( ) : base_t( ) { }
	direction( const base_t& pt ) : base_t( pt ) { }
	direction( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename ... Args>
	direction( T value, Args... values ) : base_t( value, values... ) { }


// Methods
public:

	direction<internal_t,D,internal_t> normalize( ) const {
		direction<internal_t,D> result;
		internal_t length = length( );

		for( unsigned int i = 0; i < D; ++i ) {
			result[i] = static_cast<internal_t>(base_t::m_data[i]) / length;
		}
		return result;
	}

	inline internal_t length( ) const {
		return static_cast<internal_t>( sqrt( length_sq( ) ) );
	}

	inline T length_sq( ) const {
		T length = 0;
		for( unsigned int i = 0; i < D; ++i ) {
			length += base_t::m_data[i] * base_t::m_data[i];
			assert( length > base_t::m_data[i] ); // quick code to catch some overflows
		}
		return length;
	}

}; // End class direction<T,D,INTERNAL>


template<typename T, typename INTERNAL>
class direction<T,2,INTERNAL> : public point_base<T,2> {
// Typedefs
protected:

	typedef point_base<T,2> base_t;
	typedef INTERNAL        internal_t;


// Constructors
public:

	direction( ) : base_t( ) { }
	direction( const base_t& pt ) : base_t( pt ) { }
	direction( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename ... Args>
	direction( T value, Args... values ) : base_t( value, values... ) { }


// Methods
public:

	direction<internal_t,2> normalize( ) const {
		internal_t length = length( );
		return direction<internal_t,2> {
		    static_cast<internal_t>(base_t::m_data[0]) / length,
		    static_cast<internal_t>(base_t::m_data[1]) / length
		};
	}

	inline internal_t length( ) const {
		return static_cast<internal_t>( sqrt( length_sq( ) ) );
	}

	inline T length_sq( ) const {
		return base_t::m_data[0] * base_t::m_data[0] + base_t::m_data[1] * base_t::m_data[1];
	}

	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }


// Operators
public:

	bool operator == ( const direction<T,2>& pt ) const {
		internal_t slope1, slope2;
		slope1 = static_cast<internal_t>(base_t::m_data[0]) /
		         static_cast<internal_t>(base_t::m_data[1]);
		slope2 = static_cast<internal_t>(pt[0]) / static_cast<internal_t>(pt[1]);
		return equal( slope1, slope2 );
	}

	bool operator != ( const direction<T,2>& pt ) const {
		return !(*this == pt);
	}

}; // End class direction<T,2>


template<typename T, typename INTERNAL>
class direction<T,3,INTERNAL> : public point_base<T,3> {
// Typedefs
protected:

	typedef point_base<T,3> base_t;
	typedef INTERNAL        internal_t;


// Constructors
public:

	direction( ) : base_t( ) { }
	direction( const base_t& pt ) : base_t( pt ) { }
	direction( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename ... Args>
	direction( T value, Args... values ) : base_t( value, values... ) { }


// Methods
public:

	direction<internal_t,3> normalize( ) const {
		internal_t length = length( );
		return direction<internal_t,3> {
		    static_cast<internal_t>(base_t::m_data[0]) / length,
		    static_cast<internal_t>(base_t::m_data[1]) / length,
		    static_cast<internal_t>(base_t::m_data[2]) / length
		};
	}

	inline internal_t length( ) const {
		return static_cast<internal_t>( sqrt( length_sq( ) ) );
	}

	inline T length_sq( ) const {
		return base_t::m_data[0] * base_t::m_data[0] +
		       base_t::m_data[1] * base_t::m_data[1] +
		       base_t::m_data[2] * base_t::m_data[2];
	}

	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }
	T   z( ) const { return base_t::m_data[2]; }

}; // End class direction<T,3,INTERNAL>


template<typename T, typename INTERNAL>
class direction<T,4,INTERNAL> : public point_base<T,4> {
// Typedefs
protected:

	typedef point_base<T,4> base_t;
	typedef INTERNAL        internal_t;


// Constructors
public:

	direction( ) : base_t( ) { }
	direction( const base_t& pt ) : base_t( pt ) { }
	direction( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename ... Args>
	direction( T value, Args... values ) : base_t( value, values... ) { }


// Methods
public:

	direction<internal_t,4> normalize( ) const {
		internal_t length = length( );
		return direction<internal_t,4> {
		    static_cast<internal_t>(base_t::m_data[0]) / length,
		    static_cast<internal_t>(base_t::m_data[1]) / length,
		    static_cast<internal_t>(base_t::m_data[2]) / length,
		    static_cast<internal_t>(base_t::m_data[3]) / length
		};
	}

	inline internal_t length( ) const {
		return static_cast<internal_t>( sqrt( length_sq( ) ) );
	}

	inline T length_sq( ) const {
		return base_t::m_data[0] * base_t::m_data[0] +
		       base_t::m_data[1] * base_t::m_data[1] +
		       base_t::m_data[2] * base_t::m_data[2] +
		       base_t::m_data[3] * base_t::m_data[3];
	}

	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }
	T   z( ) const { return base_t::m_data[2]; }
	T   w( ) const { return base_t::m_data[3]; }

}; // End class direction<T,4,INTERNAL>

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

