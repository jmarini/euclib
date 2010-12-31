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

#ifndef EUBLIB_VECTOR_HPP
#define EUBLIB_VECTOR_HPP

#include <boost/type_traits/is_floating_point.hpp>
#include "euclib_math.hpp"
#include "point.hpp"

namespace euclib {

template<typename T, unsigned int D>
class vector : public point_base<T,D> {
// Typedefs
protected:

	typedef point_base<T,D> base_t;


// Constructors
public:

	vector( ) : base_t( ) { }
	vector( const base_t& pt ) : base_t( pt ) { }
	vector( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename L, typename R>
	vector( const sum<L,R,T>& expr ) : base_t( expr ) { }
	template<typename L>
	vector( const product<L,T>& expr ) : base_t( expr ) { }
	template<typename L, typename R>
	vector( const difference<L,R,T>& expr ) : base_t( expr ) { }
	template<typename ... Args>
	vector( T value, Args... values ) : base_t( value, values... ) { }


// Methods
public:

	vector<T,D> normalize( ) const {
		vector<T,D> result;
		T length = length( );

		for( unsigned int i = 0; i < D; ++i ) {
			result[i] = base_t::m_data[i] / length;
		}
		return result;
	}

	T dot( const vector<T,D>& pt ) {
		T sum = 0;
		for( unsigned int i = 0; i < D; ++i ) {
			sum += base_t::m_data[i] * pt.m_data[i];
		}
		return sum;
	}

	inline T length( ) const { return sqrt( length_sq( ) ); }

	inline T length_sq( ) const {
		T length = 0;
		for( unsigned int i = 0; i < D; ++i ) {
			length += base_t::m_data[i] * base_t::m_data[i];
			assert( length > base_t::m_data[i] ); // quick code to catch some overflows
		}
		return length;
	}

}; // End class vector<T,D>


template<typename T>
class vector<T,2> : public point_base<T,2> {
// Typedefs
protected:

	typedef point_base<T,2> base_t;


// Constructors
public:

	vector( ) : base_t( ) { }
	vector( const base_t& pt ) : base_t( pt ) { }
	vector( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename L, typename R>
	vector( const sum<L,R,T>& expr ) : base_t( expr ) { }
	template<typename L>
	vector( const product<L,T>& expr ) : base_t( expr ) { }
	template<typename L, typename R>
	vector( const difference<L,R,T>& expr ) : base_t( expr ) { }
	template<typename ... Args>
	vector( T value, Args... values ) : base_t( value, values... ) { }


// Methods
public:

	T&  x( ) { return base_t::m_data[0]; }
	T&  y( ) { return base_t::m_data[1]; }
	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }


	vector<T,2> normalize( ) const {
		T length = length( );
		return vector<T,2> {
		    base_t::m_data[0] / length,
		    base_t::m_data[1] / length
		};
	}

	T dot( const vector<T,2>& pt ) {
		return base_t::m_data[0] * pt.m_data[0] + base_t::m_data[1] * pt.m_data[1];
	}

	T cross( const vector<T,2>& pt ) const {
		return base_t::m_data[0] * pt.m_data[1] - base_t::m_data[1] * pt.m_data[0];
	}

	inline T length( ) const { return sqrt( length_sq( ) ); }

	inline T length_sq( ) const {
		return base_t::m_data[0] * base_t::m_data[0] +
		       base_t::m_data[1] * base_t::m_data[1];
	}

	const T* to_gl( ) const { return base_t::m_data.data( ); }

// Operators
public:

	bool operator == ( const vector<T,2>& pt ) const {
		T slope1, slope2;
		slope1 = base_t::m_data[0] / base_t::m_data[1];
		slope2 = pt[0] / pt[1];
		return equal( slope1, slope2 );
	}

	bool operator != ( const vector<T,2>& pt ) const {
		return !(*this == pt);
	}

}; // End class vector<T,2>


template<typename T>
class vector<T,3> : public point_base<T,3> {
// Typedefs
protected:

	typedef point_base<T,3> base_t;


// Constructors
public:

	vector( ) : base_t( ) { }
	vector( const base_t& pt ) : base_t( pt ) { }
	vector( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename L, typename R>
	vector( const sum<L,R,T>& expr ) : base_t( expr ) { }
	template<typename L>
	vector( const product<L,T>& expr ) : base_t( expr ) { }
	template<typename L, typename R>
	vector( const difference<L,R,T>& expr ) : base_t( expr ) { }
	vector( T x ) : base_t( x ) { }
	vector( T x, T y ) : base_t( x, y ) { }
	vector( T x, T y, T z ) : base_t( x, y, z ) { }


// Methods
public:

	T&  x( ) { return base_t::m_data[0]; }
	T&  y( ) { return base_t::m_data[1]; }
	T&  z( ) { return base_t::m_data[2]; }
	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }
	T   z( ) const { return base_t::m_data[2]; }


	vector<T,3> normalize( ) const {
		T length = length( );
		return vector<T,3> {
		    base_t::m_data[0] / length,
		    base_t::m_data[1] / length,
		    base_t::m_data[2] / length
		};
	}

	T dot( const vector<T,3>& pt ) {
		return base_t::m_data[0] * pt.m_data[0] +
		       base_t::m_data[1] * pt.m_data[1] +
		       base_t::m_data[2] * pt.m_data[2];
	}

	vector<T,3> cross( const vector<T,3>& pt ) const {
		vector<T,3> result;
		result.m_data[0] = base_t::m_data[1] * pt.m_data[2] -
		                   base_t::m_data[2] * pt.m_data[1];
		result.m_data[1] = base_t::m_data[0] * pt.m_data[2] -
		                   base_t::m_data[2] * pt.m_data[0];
		result.m_data[2] = base_t::m_data[0] * pt.m_data[1] -
		                   base_t::m_data[1] * pt.m_data[0];
		return result;
	}

	inline T length( ) const { return sqrt( length_sq( ) ); }

	inline T length_sq( ) const {
		return base_t::m_data[0] * base_t::m_data[0] +
		       base_t::m_data[1] * base_t::m_data[1] +
		       base_t::m_data[2] * base_t::m_data[2];
	}

	const T* to_gl( ) const { return base_t::m_data.data( ); }

}; // End class vector<T,3>


template<typename T>
class vector<T,4> : public point_base<T,4> {
// Typedefs
protected:

	typedef point_base<T,4> base_t;


// Constructors
public:

	vector( ) : base_t( ) { }
	vector( const base_t& pt ) : base_t( pt ) { }
	vector( base_t&& pt ) : base_t( std::forward<base_t>( pt ) ) { }
	template<typename L, typename R>
	vector( const sum<L,R,T>& expr ) : base_t( expr ) { }
	template<typename L>
	vector( const product<L,T>& expr ) : base_t( expr ) { }
	template<typename L, typename R>
	vector( const difference<L,R,T>& expr ) : base_t( expr ) { }
	vector( T x ) : base_t( x ) { }
	vector( T x, T y ) : base_t( x, y ) { }
	vector( T x, T y, T z ) : base_t( x, y, z ) { }
	vector( T x, T y, T z, T w ) : base_t( x, y, z, w ) { }


// Methods
public:

	T&  x( ) { return base_t::m_data[0]; }
	T&  y( ) { return base_t::m_data[1]; }
	T&  z( ) { return base_t::m_data[2]; }
	T&  w( ) { return base_t::m_data[3]; }
	T   x( ) const { return base_t::m_data[0]; }
	T   y( ) const { return base_t::m_data[1]; }
	T   z( ) const { return base_t::m_data[2]; }
	T   w( ) const { return base_t::m_data[3]; }


	vector<T,4> normalize( ) const {
		T length = length( );
		return vector<T,4> {
		    base_t::m_data[0] / length,
		    base_t::m_data[1] / length,
		    base_t::m_data[2] / length,
		    base_t::m_data[3] / length
		};
	}

	T dot( const vector<T,4>& pt ) {
		return base_t::m_data[0] * pt.m_data[0] +
		       base_t::m_data[1] * pt.m_data[1] +
		       base_t::m_data[2] * pt.m_data[2] +
		       base_t::m_data[3] * pt.m_data[3];
	}

	inline T length( ) const { return sqrt( length_sq( ) ); }

	inline T length_sq( ) const {
		return base_t::m_data[0] * base_t::m_data[0] +
		       base_t::m_data[1] * base_t::m_data[1] +
		       base_t::m_data[2] * base_t::m_data[2] +
		       base_t::m_data[3] * base_t::m_data[3];
	}

	const T* to_gl( ) const { return base_t::m_data.data( ); }

}; // End class vector<T,4>


// Specializations to compute expressions
template<typename T, unsigned int D>
class container<vector<T,D>,T> {
	const vector<T,D>& m_obj;

public:
	container( const vector<T,D>& obj ) : m_obj( obj ) { }
	T operator [] ( unsigned int i ) const { return m_obj[i]; }
};

template<typename T>
class container<vector<T,2>,T> {
	const vector<T,2>& m_obj;

public:
	container( const vector<T,2>& obj ) : m_obj( obj ) { }
	T operator [] ( unsigned int i ) const { return m_obj[i]; }
};

template<typename T>
class container<vector<T,3>,T> {
	const vector<T,3>& m_obj;

public:
	container( const vector<T,3>& obj ) : m_obj( obj ) { }
	T operator [] ( unsigned int i ) const { return m_obj[i]; }
};

template<typename T>
class container<vector<T,4>,T> {
	const vector<T,4>& m_obj;

public:
	container( const vector<T,4>& obj ) : m_obj( obj ) { }
	T operator [] ( unsigned int i ) const { return m_obj[i]; }
};


// Various typedefs to make usage easier
typedef vector<float,2>         vector2f;
typedef vector<float,3>         vector3f;
typedef vector<float,4>         vector4f;

typedef vector<double,2>        vector2d;
typedef vector<double,3>        vector3d;
typedef vector<double,4>        vector4d;

}  // End namespace euclib

#endif // EUCLIB_VECTOR_HPP

