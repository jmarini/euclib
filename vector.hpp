/*
 *	Copyright (C) 2010-2011 Jonathan Marini
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef EUBLIB_VECTOR_HPP
#define EUBLIB_VECTOR_HPP

#include <array>
#include <iterator>

#ifndef __GNUC__
	#include <algorithm>
	#include <initializer_list>
#else // workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=49251
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "type_traits.hpp"
#include "euclib_math.hpp"


namespace euclib {

namespace detail {

template<typename T, std::size_t D>
class vector_base {
// Typedefs
protected:

	typedef std::numeric_limits<T>  limit_type;
	typedef vector_base<T,D>        data_type;

	// This class can only be used with floating point types
	static_assert( std::is_floating_point<T>::value, "T must be floating point" );
	static_assert( D != 0, "Cannot have 0-dimensional object" );
	static_assert( limit_type::is_specialized, "T must specialize std::numeric_limits" );


public:

	typedef T                                      value_type;
	typedef T*                                     pointer;
	typedef const T*                               const_pointer;
	typedef T&                                     reference;
	typedef const T&                               const_reference;
	typedef pointer                                iterator;
	typedef const_pointer                          const_iterator;
	typedef std::reverse_iterator<iterator>        reverse_iterator;
	typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;
	typedef std::size_t                            size_type;
	typedef std::ptrdiff_t                         difference_type;

// Methods
public:

	constexpr size_type  dimension( ) const noexcept { return D; }
	constexpr size_type  size( ) const noexcept { return D; }
	constexpr size_type  max_size( ) const noexcept { return D; }
	constexpr bool       empty( ) const noexcept { return false; } // Can never be size 0

};

} // End namespace detail




//
// Generic vector<T,D> for D > 4
//
template<typename T, const std::size_t D>
class vector : public detail::vector_base<T,D> {
// Typedefs
private:

	typedef detail::vector_base<T,D>        base_type;
	typedef typename base_type::limit_type  limit_type;
	typedef vector<T,D>                     data_type;
	typedef std::array<T,D>                 array_type;


public:

	typedef typename base_type::value_type              value_type;
	typedef typename base_type::pointer                 pointer;
	typedef typename base_type::const_pointer           const_pointer;
	typedef typename base_type::reference               reference;
	typedef typename base_type::const_reference         const_reference;
	typedef typename base_type::iterator                iterator;
	typedef typename base_type::const_iterator          const_iterator;
	typedef typename base_type::reverse_iterator        reverse_iterator;
	typedef typename base_type::const_reverse_iterator  const_reverse_iterator;
	typedef typename base_type::size_type               size_type;
	typedef typename base_type::difference_type         difference_type;


// Variables
private:

	array_type  m_data;


// Constructors
public:

	vector( ) { }
	vector( const data_type& vec ) { *this = vec; }
	vector( data_type&& vec ) { *this = std::move(vec); }
	
	vector( const array_type& data ) {
		std::copy( data.begin( ), data.end( ), begin( ) );
	}
	vector( array_type&& data ) { m_data.swap( data ); }

#ifdef __GNUC__
	template<typename... Args>
	vector( value_type value, Args... values ) {
		static_assert( sizeof...(values) < D,
		               "too many arguments to constructor" );
		fill_values( 0, value, values... );

	}
#else
	// use initializer list with runtime assert
	vector( std::initializer_list<value_type> values ) {
		assert( values.size( ) <= D && "too many arguments to constructor" );
		std::fill( std::copy( values.begin( ), values.end( ), m_data.begin( ) ),
		           m_data.end( ), value_type( ) );
	}
#endif

	// expression constructor ...


// Methods
private:

#ifdef __GNUC__
	template<typename... Args>
	void fill_values( size_type i, value_type value, Args... values ) {
		m_data[i] = value;
		fill_values( i + 1, values... );
	}

	void fill_values( size_type i ) {
		std::fill( typename array_type::iterator( &m_data[i] ), m_data.end( ), value_type( ) );
	}
#endif


public:

	data_type  normalize( ) const {
		data_type vec = *this;
		value_type len = length( );
		std::for_each( vec.m_data.begin( ), vec.m_data.end( ),
			[&]( value_type& v ) {
				v /= len;
		});
		return vec;
	}
	void       normalize_ip( ) {
		value_type len = length( );
		std::for_each( begin( ), end( ),
			[&]( value_type& v ) {
				v /= len;
		});
	}

	value_type  dot( const data_type& vec ) const {
		value_type num = value_type( );
		for( size_type i = 0; i < D; ++i ) {
			num += m_data[i] * vec[i];
		}
		return num;
	}

	
	value_type  length( ) const { return std::sqrt( length_sq( ) ); }
	value_type  length_sq( ) const { // TODO: possible overflow...
		value_type len = value_type( );
		std::for_each( begin( ), end( ),
			[&]( value_type v ) {
				len += v * v;
		});
		return len;
	}

	pointer        data( ) { return m_data.data( ); }
	const_pointer  data( ) const { return m_data.data( ); }

	void  fill( const_reference value ) { std::fill( begin( ), end( ), value ); }
	void  swap( data_type& vec ) { m_data.swap( vec.m_data ); }

	// iterators...
	iterator                begin( ) { return m_data.begin( ); }
	const_iterator          begin( ) const { return m_data.begin( ); }
	iterator                end( ) { return m_data.end( ); }
	const_iterator          end( ) const { return m_data.end( ); }

	reverse_iterator        rbegin( ) { return m_data.rbegin( ); }
	const_reverse_iterator  rbegin( ) const { return m_data.rbegin( ); }
	reverse_iterator        rend( ) { return m_data.rend( ); }
	const_reverse_iterator  rend( ) const { return m_data.rend( ); }

	const_iterator          cbegin( ) const { return m_data.cbegin( ); }
	const_iterator          cend( ) const { return m_data.cend( ); }
	const_reverse_iterator  crbegin( ) const { return m_data.crbegin( ); }
	const_reverse_iterator  crend( ) const { return m_data.crend( ); }


// Operators
public:

	reference        operator [] ( size_type i ) { return m_data[i]; }
	const_reference  operator [] ( size_type i ) const { return m_data[i]; }

	data_type&  operator = ( const data_type& vec ) {
		std::copy( vec.begin( ), vec.end( ), begin( ) );
		return *this;
	}
	data_type&  operator = ( data_type&& vec ) {
		m_data.swap( vec.m_data );
		return *this;
	}
	
	data_type&  operator += ( const data_type& vec ) {
		for( std::size_t i = 0; i < D; ++i ) {
			m_data[i] += vec[i];
		}
		return *this;
	}
	data_type&  operator *= ( value_type scalar ) {
		std::for_each( begin( ), end( ),
			[&]( value_type& v ) {
				v *= scalar;
		});
		return *this;
	}

	// expression assignment


};

template<typename T, std::size_t D>
bool operator == ( const vector<T,D>& lhs, const vector<T,D>& rhs ) {
	auto itr = std::mismatch( lhs.begin( ), lhs.end( ), rhs.begin( ), equal<T> );
	return itr.first == lhs.end( ) && itr.second == rhs.end( );
}

template<typename T, std::size_t L, std::size_t R> inline // different size vectors always not equal
constexpr bool operator == ( const vector<T,L>& lhs, const vector<T,R>& rhs ) { return false; }

template<typename T, std::size_t D> inline
bool operator != ( const vector<T,D>& lhs, const vector<T,D>& rhs ) { return !(lhs == rhs); }

template<typename T, std::size_t D> inline
T  dot( const vector<T,D>& lhs, const vector<T,D>& rhs ) { return lhs.dot( rhs ); }




//
// vector<T,2>, with (x,y)(r,g)(s,t) accessors
//
template<typename T>
class vector<T,2> : public detail::vector_base<T,2> {
// Typedefs
private:

	typedef detail::vector_base<T,2>        base_type;
	typedef typename base_type::limit_type  limit_type;
	typedef vector<T,2>                     data_type;
	typedef std::array<T,2>                 array_type;


public:

	typedef typename base_type::value_type              value_type;
	typedef typename base_type::pointer                 pointer;
	typedef typename base_type::const_pointer           const_pointer;
	typedef typename base_type::reference               reference;
	typedef typename base_type::const_reference         const_reference;
	typedef typename base_type::iterator                iterator;
	typedef typename base_type::const_iterator          const_iterator;
	typedef typename base_type::reverse_iterator        reverse_iterator;
	typedef typename base_type::const_reverse_iterator  const_reverse_iterator;
	typedef typename base_type::size_type               size_type;
	typedef typename base_type::difference_type         difference_type;


// Variables
public:

	union { value_type  x, r, s; };
	union { value_type  y, g, t; };


// Constructors
public:

	vector( const data_type& vec ) { *this = vec; }
	vector( data_type&& vec ) { *this = std::move(vec); }
	
	vector( const array_type& data )
		: x(data[0]), y(data[1]) { }
	vector( array_type&& data ) {
		std::swap(x,data[0]), std::swap(y,data[1]);
	}

	vector( value_type first = value_type( ), value_type second = value_type( ) )
		: x(first), y(second) { }

	// expression constructor ...


// Methods
public:

	data_type  normalize( ) const {
		data_type vec = *this;
		value_type len = length( );
		vec.x /= len;
		vec.y /= len;
		return vec;
	}
	void       normalize_ip( ) {
		value_type len = length( );
		x /= len;
		y /= len;
	}

	value_type  dot( const data_type& vec ) const {
		return x*vec.x + y*vec.y;
	}
	value_type  cross( const data_type& vec ) const {
		return x*vec.y - y*vec.x;
	}

	
	value_type  length( ) const { return std::sqrt( length_sq( ) ); }
	value_type  length_sq( ) const {
		return x*x + y*y;
	}

	pointer        data( ) { return &x; }
	const_pointer  data( ) const { return &x; }

	void  fill( const_reference value ) { x = value; y = value; }
	void  swap( data_type& vec ) {
		std::swap( x, vec.x );
		std::swap( y, vec.y );
	}

	// iterators...
	iterator                begin( ) { return iterator( &x ); }
	const_iterator          begin( ) const { return const_iterator( &x ); }
	iterator                end( ) { return iterator( &(&x)[2] ); }
	const_iterator          end( ) const { return const_iterator( &(&x)[2] ); }

	reverse_iterator        rbegin( ) { return reverse_iterator(end( )); }
	const_reverse_iterator  rbegin( ) const { return const_reverse_iterator(end( )); }
	reverse_iterator        rend( ) { return reverse_iterator(begin( )); }
	const_reverse_iterator  rend( ) const { return const_reverse_iterator(begin( )); }

	const_iterator          cbegin( ) const { return const_iterator( &x ); }
	const_iterator          cend( ) const { return const_iterator( &(&x)[2] ); }
	const_reverse_iterator  crbegin( ) const { return const_reverse_iterator(end( )); }
	const_reverse_iterator  crend( ) const { return const_reverse_iterator(begin( )); }


// Operators
public:

	reference        operator [] ( size_type i ) { return (&x)[i]; }
	const_reference  operator [] ( size_type i ) const { return (&x)[i]; }

	data_type&  operator = ( const data_type& vec ) {
		x = vec.x;
		y = vec.y;
		return *this;
	}
	data_type&  operator = ( data_type&& vec ) {
		swap( vec );
		return *this;
	}

	data_type&  operator += ( const data_type& vec ) {
		x += vec.x;
		y += vec.y;
		return *this;
	}
	data_type&  operator *= ( value_type scalar ) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	// expression assignment


}; // End class vector<T,2>

template<typename T> inline
T  cross( const vector<T,2>& lhs, const vector<T,2>& rhs ) { return lhs.cross( rhs ); }




//
// vector<T,3>, with (x,y,z)(r,g,b)(s,t,p) accessors
//
template<typename T>
class vector<T,3> : public detail::vector_base<T,3> {
// Typedefs
private:

	typedef detail::vector_base<T,3>        base_type;
	typedef typename base_type::limit_type  limit_type;
	typedef vector<T,3>                     data_type;
	typedef std::array<T,3>                 array_type;


public:

	typedef typename base_type::value_type              value_type;
	typedef typename base_type::pointer                 pointer;
	typedef typename base_type::const_pointer           const_pointer;
	typedef typename base_type::reference               reference;
	typedef typename base_type::const_reference         const_reference;
	typedef typename base_type::iterator                iterator;
	typedef typename base_type::const_iterator          const_iterator;
	typedef typename base_type::reverse_iterator        reverse_iterator;
	typedef typename base_type::const_reverse_iterator  const_reverse_iterator;
	typedef typename base_type::size_type               size_type;
	typedef typename base_type::difference_type         difference_type;


// Variables
public:

	union { value_type  x, r, s; };
	union { value_type  y, g, t; };
	union { value_type  z, b, p; };


// Constructors
public:

	vector( const data_type& vec ) { *this = vec; }
	vector( data_type&& vec ) { *this = std::move(vec); }

	vector( const vector<T,2>& vec, value_type third )
		: x(vec.x), y(vec.y), z(third) { }
	vector( value_type first, const vector<T,2>& vec )
		: x(first), y(vec.x), z(vec.y) { }
	
	vector( const array_type& data )
		: x(data[0]), y(data[1]), z(data[2]) { }
	vector( array_type&& data ) {
		std::swap(x,data[0]), std::swap(y,data[1]); std::swap(z,data[2]);
	}

	vector( value_type first = value_type( ), value_type second = value_type( ),
	        value_type third = value_type( ) )
		: x(first), y(second), z(third) { }

	// expression constructor ...


// Methods
public:

	data_type  normalize( ) const {
		data_type vec = *this;
		value_type len = length( );
		vec.x /= len;
		vec.y /= len;
		vec.z /= len;
		return vec;
	}
	void       normalize_ip( ) {
		value_type len = length( );
		x /= len;
		y /= len;
		z /= len;
	}

	value_type  dot( const data_type& vec ) const {
		return x*vec.x + y*vec.y + z*vec.z;
	}
	data_type   cross( const data_type& vec ) const {
		data_type tmp;
		tmp.x = y*vec.z - z*vec.y;
		tmp.y = z*vec.x - x*vec.z;
		tmp.z = x*vec.y - y*vec.x;
		return tmp;
	}
	data_type   vector_triple( const data_type& second, const data_type& third ) const {
		return this->cross( second.cross( third ) );
	}
	value_type  scalar_triple( const data_type& second, const data_type& third ) const {
		return this->dot( second.cross( third ) );
	}

	
	value_type  length( ) const { return std::sqrt( length_sq( ) ); }
	value_type  length_sq( ) const {
		return x * x + y * y + z * z;
	}

	pointer        data( ) { return &x; }
	const_pointer  data( ) const { return &x; }

	void  fill( const_reference value ) { x = value; y = value; z = value; }
	void  swap( data_type& vec ) {
		std::swap( x, vec.x );
		std::swap( y, vec.y );
		std::swap( z, vec.z );
	}

	// iterators...
	iterator                begin( ) { return iterator( &x ); }
	const_iterator          begin( ) const { return const_iterator( &x ); }
	iterator                end( ) { return iterator( &(&x)[3] ); }
	const_iterator          end( ) const { return const_iterator( &(&x)[3] ); }

	reverse_iterator        rbegin( ) { return reverse_iterator(end( )); }
	const_reverse_iterator  rbegin( ) const { return const_reverse_iterator(end( )); }
	reverse_iterator        rend( ) { return reverse_iterator(begin( )); }
	const_reverse_iterator  rend( ) const { return const_reverse_iterator(begin( )); }

	const_iterator          cbegin( ) const { return const_iterator( &x ); }
	const_iterator          cend( ) const { return const_iterator( &(&x)[3] ); }
	const_reverse_iterator  crbegin( ) const { return const_reverse_iterator(end( )); }
	const_reverse_iterator  crend( ) const { return const_reverse_iterator(begin( )); }


// Operators
public:

	reference        operator [] ( size_type i ) { return (&x)[i]; }
	const_reference  operator [] ( size_type i ) const { return (&x)[i]; }

	data_type&  operator = ( const data_type& vec ) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		return *this;
	}
	data_type&  operator = ( data_type&& vec ) {
		swap( vec );
		return *this;
	}

	data_type&  operator += ( const data_type& vec ) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}
	data_type&  operator *= ( value_type scalar ) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	// expression assignment


}; // End class vector<T,3>

template<typename T> inline
vector<T,3>  cross( const vector<T,3>& lhs, const vector<T,3>& rhs ) { return lhs.cross( rhs ); }

template<typename T> inline
vector<T,3>  vector_triple( const vector<T,3>& first, const vector<T,3>& second,
                            const vector<T,3>& third ) {
	return first.vector_triple( second, third );
}

template<typename T> inline
T  scalar_triple( const vector<T,3>& first, const vector<T,3>& second,
                            const vector<T,3>& third ) {
	return first.scalar_triple( second, third );
}




//
// vector<T,4>, with (x,y,z,w)(r,g,b,a)(s,t,p,q) accessors
//
template<typename T>
class vector<T,4> : public detail::vector_base<T,4> {
// Typedefs
private:

	typedef detail::vector_base<T,4>        base_type;
	typedef typename base_type::limit_type  limit_type;
	typedef vector<T,4>                     data_type;
	typedef std::array<T,4>                 array_type;


public:

	typedef typename base_type::value_type              value_type;
	typedef typename base_type::pointer                 pointer;
	typedef typename base_type::const_pointer           const_pointer;
	typedef typename base_type::reference               reference;
	typedef typename base_type::const_reference         const_reference;
	typedef typename base_type::iterator                iterator;
	typedef typename base_type::const_iterator          const_iterator;
	typedef typename base_type::reverse_iterator        reverse_iterator;
	typedef typename base_type::const_reverse_iterator  const_reverse_iterator;
	typedef typename base_type::size_type               size_type;
	typedef typename base_type::difference_type         difference_type;


// Variables
public:

	union { value_type  x, r, s; };
	union { value_type  y, g, t; };
	union { value_type  z, b, p; };
	union { value_type  w, a, q; };


// Constructors
public:

	vector( const data_type& vec ) { *this = vec; }
	vector( data_type&& vec ) { *this = std::move(vec); }

	vector( const vector<T,2>& first_vec, const vector<T,2>& second_vec )
		: x(first_vec.x), y(first_vec.y), z(second_vec.x), w(second_vec.y) { }
	vector( const vector<T,2>& vec, value_type third, value_type fourth )
		: x(vec.x), y(vec.y), z(third), w(fourth) { }
	vector( value_type first, const vector<T,2>& vec, value_type fourth )
		: x(first), y(vec.x), z(vec.y), w(fourth) { }
	vector( value_type first, value_type second, const vector<T,2>& vec )
		: x(first), y(second), z(vec.x), w(vec.y) { }
	vector( const vector<T,3>& vec, value_type& fourth )
		: x(vec.x), y(vec.y), z(vec.z), w(fourth ) { }
	vector( value_type first, const vector<T,3>& vec )
		: x(first), y(vec.x), z(vec.y), w(vec.z) { }
	
	vector( const array_type& data )
		: x(data[0]), y(data[1]), z(data[2]), w(data[3]) { }
	vector( array_type&& data ) {
		std::swap(x,data[0]), std::swap(y,data[1]); std::swap(z,data[2]); std::swap(w,data[3]);
	}

	vector( value_type first = value_type( ), value_type second = value_type( ),
	        value_type third = value_type( ), value_type fourth = value_type( ) )
		: x(first), y(second), z(third), w(fourth) { }

	// expression constructor ...


// Methods
public:

	data_type  normalize( ) const {
		data_type vec = *this;
		value_type len = length( );
		vec.x /= len;
		vec.y /= len;
		vec.z /= len;
		vec.w /= len;
		return vec;
	}
	void       normalize_ip( ) {
		value_type len = length( );
		x /= len;
		y /= len;
		z /= len;
		w /= len;
	}

	value_type  inner_product( ) const { return length_sq( ); }
	value_type  scalar_product( ) const { return inner_product( ); }
	value_type  dot( ) const { return inner_product( ); }

	
	value_type  length( ) const { return std::sqrt( length_sq( ) ); }
	value_type  length_sq( ) const {
		return x * x + y * y + z * z + w * w;
	}

	pointer        data( ) { return &x; }
	const_pointer  data( ) const { return &x; }

	void  fill( const_reference value ) { x = value; y = value; z = value; w = value; }
	void  swap( data_type& vec ) {
		std::swap( x, vec.x );
		std::swap( y, vec.y );
		std::swap( z, vec.z );
		std::swap( w, vec.w );
	}

	// iterators...
	iterator                begin( ) { return iterator( &x ); }
	const_iterator          begin( ) const { return const_iterator( &x ); }
	iterator                end( ) { return iterator( &(&x)[4] ); }
	const_iterator          end( ) const { return const_iterator( &(&x)[4] ); }

	reverse_iterator        rbegin( ) { return reverse_iterator(end( )); }
	const_reverse_iterator  rbegin( ) const { return const_reverse_iterator(end( )); }
	reverse_iterator        rend( ) { return reverse_iterator(begin( )); }
	const_reverse_iterator  rend( ) const { return const_reverse_iterator(begin( )); }

	const_iterator          cbegin( ) const { return const_iterator( &x ); }
	const_iterator          cend( ) const { return const_iterator( &(&x)[4] ); }
	const_reverse_iterator  crbegin( ) const { return const_reverse_iterator(end( )); }
	const_reverse_iterator  crend( ) const { return const_reverse_iterator(begin( )); }


// Operators
public:

	reference        operator [] ( size_type i ) { return (&x)[i]; }
	const_reference  operator [] ( size_type i ) const { return (&x)[i]; }

	data_type&  operator = ( const data_type& vec ) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.w;
		return *this;
	}
	data_type&  operator = ( data_type&& vec ) {
		swap( vec );
		return *this;
	}

	data_type&  operator += ( const data_type& vec ) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
		return *this;
	}
	data_type&  operator *= ( value_type scalar ) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	// expression assignment


}; // End class vector<T,4>


// Various typedefs to make usage easier
typedef vector<float,2>       vector2f;
typedef vector<float,3>       vector3f;
typedef vector<float,4>       vector4f;

typedef vector<float,2>       vector2;
typedef vector<float,3>       vector3;
typedef vector<float,4>       vector4;

typedef vector<double,2>      vector2d;
typedef vector<double,3>      vector3d;
typedef vector<double,4>      vector4d;

typedef vector<long double,2> vector2ld;
typedef vector<long double,3> vector3ld;
typedef vector<long double,4> vector4ld;

}  // End namespace euclib

#ifdef __GNUC__ // again, bug workaround
#	pragma GCC diagnostic pop
#endif

#endif // EUCLIB_VECTOR_HPP

