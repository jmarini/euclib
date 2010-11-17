/*	polygon.hpp  v 0.1.1.10.1117
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

#ifndef EUBLIB_POLYGON_HPP
#define EUBLIB_POLYGON_HPP

#include <ostream>
#include <limits>
#include <complex>
#include <vector>
#include <algorithm>
#include "point.hpp"
#include "rect.hpp"
#include "segment.hpp"

namespace euclib {

template<typename T>
class polygon2 {
// Typedefs
protected:

	typedef std::numeric_limits<T> limit_t;

	// This class can only be used with scalar types
	//   or types with a specific specialization
	static_assert( limit_t::is_specialized,
	               "type not compatible with std::numeric_limits" );

// Friend functions
public:
	// defined in euclib_helper.hpp
	template<typename T_Ex> friend
	polygon2<T_Ex> translate( const polygon2<T_Ex>& poly, T_Ex x, T_Ex y );
	template<typename T_Ex> friend
	polygon2<T_Ex> rotate( const polygon2<T_Ex>& target, const point2<T_Ex>& about, float angle, bool clockwise = true );
	template<typename T_Ex> friend
	polygon2<T_Ex> mirror( const polygon2<T_Ex>& target, const line2<T_Ex>& over );
	template<typename T_Ex> friend
	point2<T_Ex> overlap( const point2<T_Ex>& pt, const polygon2<T_Ex>& poly );
	template<typename T_Ex> friend
	line2<T_Ex> overlap( const line2<T_Ex>& line, const polygon2<T_Ex>& poly );

// Variables
private:

	std::vector<point2<T>>  m_hull;
	rect2<T>                m_bounding_box;

	static T invalid; // holds either limit_t::infinity or limit_t::max


// Constructors
public:

	polygon2( ) {
		m_hull.reserve( 3 ); // 3 is minimum to make polygon
		set_null( );
	}
	polygon2( const polygon2<T>& poly ) { *this = poly; }
	polygon2( polygon2<T>&& poly ) { *this = std::move( poly ); }
	polygon2( const std::vector<point2<T>>& points ) { add_points( points ); }

	template<typename... Points>
	polygon2( const point2<T>& point, const Points&... points ) {
		m_hull.reserve( sizeof...(points) + 1 );
		add_points( point, points... );
	}

	template<typename... Points>
	polygon2( point2<T>&& point, Points&&... points ) {
		m_hull.reserve( sizeof...(points) + 1 );
		add_points( std::forward<point2<T>>( point ),
		            std::forward<Points>( points )... );
	}


// Methods
public:

	// TODO: this is probably a good null, think about it though
	// Returns a null polygon, defined as having a null bounding box
	static polygon2<T> null( ) {
		static polygon2<T> null = polygon2<T>( );
		return null;
	}

	// TODO: I don't like these being hardcoded to floats...

	float width( ) const { return m_bounding_box.width( ); }
	float height( ) const { return m_bounding_box.height( ); }
	// TODO: look at algorithm from ../polygon.cpp
	float area( ) const {
		std::cerr << "Error: polygon<T>::area( ) not implemented.\n";
		return 0.f;
	}
	float perimeter( ) const {
		float perim = 0.f;
		for( unsigned int i = 0; i < m_hull.size( ); ++i ) {
			if( i + 1 == m_hull.size( ) ) {
				perim += line2<T>( m_hull[i], m_hull[0] ).length( );
			}
			else {
				perim += line2<T>( m_hull[i], m_hull[i+1] ).length( );
			}
		}
		return perim;
	}
	rect2<T> bounding_box( ) const { return m_bounding_box; }
	unsigned int size( ) const { return m_hull.size( ); }
	
	template<typename... Points>
	void add_points( const point2<T>& point, const Points&... points ) {
		if( point != point2<T>::null( ) ) {
			m_hull.push_back( point );
		}
		add_points( points... );
	}
	
	template<typename... Points>
	void add_points( point2<T>&& point, Points&&... points ) {
		if( point != point2<T>::null( ) ) {
			m_hull.push_back( std::forward<point2<T>>( point ) );
		}
		add_points( std::forward<Points>( points )... );
	}
	
	void add_points( const std::vector<point2<T>>& points ) {
		m_hull.reserve( points.size( ) );
		for( auto itr = points.begin( ); itr != points.end( ); ++itr ) {
			add_points( *itr );
		}
	}
	
	point2<T> operator [] ( int index ) const {
		return m_hull.at( index );
	}

	void print( std::ostream& stream, bool newline = false ) const {
		stream << "Polygon: size = " << m_hull.size( ) << "\n  ";
		for( unsigned int i = 0; i < m_hull.size( ); ++i ) {
			stream << ( i != 0 ? "->" : "" );
			m_hull[i].print( stream, false );
		}
		if( newline ) { stream << "\n"; }
	}
	
	void gnuplot( std::ostream& stream ) const {
		for( unsigned int i = 0; i < m_hull.size( ); ++i ) {
			stream << m_hull[i];
		}
		stream << m_hull[0];
		stream << "e\n";
	}

private:
	
	void add_points( ) {
		graham_hull( );
		calc_bounding_box( );
	}
	
	T direction( const point2<T>& pt0, const point2<T>& pt1, const point2<T>& pt2 ) const {
		return ( (pt1.x-pt0.x)*(pt2.y-pt0.y) - (pt1.y-pt0.y)*(pt2.x-pt0.x) );
	}

	void graham_hull( ) {
		if( m_hull.size( ) < 3 ) { return; }
		
		// holds the points of the convex hull
		std::vector<point2<T>> stack;
		stack.reserve( m_hull.size( ) );

		// find the right/bottommost point
		auto best = m_hull.begin( );
		for( auto itr = m_hull.begin( ); itr != m_hull.end( ); ++itr ) {
			if( best->y - itr->y > limit_t::epsilon( ) ) {
				best = itr;
			}
			else if( std::abs(itr->y - best->y) <= limit_t::epsilon( ) &&
			         best->x - itr->x > limit_t::epsilon( ) ) {
				best = itr;
			}
			
		}
		point2<T> tmp = *best;

		// sort points by angle hoizontally out from the point found above
		std::sort( m_hull.begin( ), m_hull.end( ),
			[tmp](point2<T> l, point2<T> r)->bool {
				if( l == tmp ) { return true; }
				else if( r == tmp ) { return false; }
				float angle1 = atan2( l.y - tmp.y, l.x - tmp.x );
				float angle2 = atan2( r.y - tmp.y, r.x - tmp.x );
				if( std::abs(angle1 - angle2) <= std::numeric_limits<float>::epsilon( ) ) {
					if( std::abs(l.x-r.x) <= limit_t::epsilon( ) ) {
						return (bool)(r.y - l.y > limit_t::epsilon( ) );
					}
					return (bool)(r.x - l.x > limit_t::epsilon( ) );
				}
				return ( angle2 - angle1 > std::numeric_limits<float>::epsilon( ) );
			}
		);

		// these points are definitely in the hull
		stack.push_back( *m_hull.begin( ) );
		stack.push_back( *(m_hull.begin( ) + 1) );
		
		// move through all points
		for( auto itr = m_hull.begin( ) + 2; itr != m_hull.end( ); ++itr ) {
			if( stack.size( ) < 2 ) { stack.push_back( *itr ); }
			else {
				// only left turns allowed
				T dir = direction( *(stack.rbegin()+1), *stack.rbegin(), *itr );
				if( dir > limit_t::epsilon( ) ) {
					stack.push_back( *itr );
				}
				else if( std::abs(dir) <= limit_t::epsilon( ) ) {
					float d1 = segment2<T>( *(stack.rbegin( )+1), *itr ).length( );
					float d2 = segment2<T>( *(stack.rbegin( )+1), *stack.rbegin( ) ).length( );
					if( d1 - d2 > std::numeric_limits<float>::epsilon( ) ) {
						stack.pop_back( );
						--itr;
					}
				}
				else {
					stack.pop_back( );
					--itr;
				}
			}
		}
		
		m_hull = stack;
	}

	void calc_bounding_box( ) {
		// best guess
		auto itr = m_hull.begin( );
		T l = itr->x;
		T r = itr->x;
		T t = itr->y;
		T b = itr->y;
		for( ++itr ; itr != m_hull.end( ); ++itr ) {
			// x
			if( l - itr->x > limit_t::epsilon( ) ) {
				l = itr->x;
			}
			else if( itr->x - r > limit_t::epsilon( ) ) {
				r = itr->x;
			}
			
			// y
			if( t - itr->y > limit_t::epsilon( ) ) {
				t = itr->y;
			}
			else if( itr->y - b > limit_t::epsilon( ) ) {
				b = itr->y;
			}
		}
		
		m_bounding_box = rect2<T>( l, r, t, b );
	}

	void check_valid( ) {
		auto itr = m_hull.begin( );
		while( itr != m_hull.end( ) ) {
			if( *itr == point2<T>::null( ) ) {
				itr = m_hull.erase( itr );
			}
			else { ++itr; }
		}
		if( m_hull.size( ) < 3 ) {
			set_null( );
		}
	}
	
	void set_null( ) {
		m_bounding_box = rect2<T>::null( );
	}


// Operators
public:

	bool operator == ( const polygon2<T>& poly ) const {
		// quick test for failure
		if( m_bounding_box != poly.m_bounding_box ) { return false; }
		// test for null
		else if( m_bounding_box == rect2<T>::null( ) ) { return true; }
		// test size of hull
		else if( m_hull.size( ) != poly.m_hull.size( ) ) { return false; }
		// test every point
		else {
			// graham_hull( ) should have sorted these the same way
			for( unsigned int i = 0; i < m_hull.size( ); ++i ) {
				if( m_hull[i] != poly.m_hull[i] ) { return false; }
			}
			return true;
		}
	}
	
	bool operator != ( const polygon2<T>& poly ) const {
		return !(*this == poly);
	}
	
	polygon2<T>& operator = ( const polygon2<T>& poly ) {
		m_bounding_box = poly.m_bounding_box;
		m_hull = poly.m_hull;
		return *this;
	}
	
	polygon2<T>& operator = ( polygon2<T>&& poly ) {
		m_bounding_box = poly.m_bounding_box;
		std::swap( m_hull, poly.m_hull );
		return *this;
	}
	
	friend std::ostream& operator << ( std::ostream& stream, const polygon2<T>& poly ) {
		#ifdef GNUPLOT
			for( unsigned int i = 0; i < poly.m_hull.size( ); ++i ) {
				stream << poly.m_hull[i];
			}
			stream << poly.m_hull[0];
			return stream << "e\n";
		#else
			stream << "Polygon: size = " << poly.m_hull.size( ) << "\n  ";
			for( unsigned int i = 0; i < poly.m_hull.size( ); ++i ) {
				stream << ( i != 0 ? "->" : "" ) << poly.m_hull[i];
			}
			return stream;
		#endif
	}


}; // End class polygon2<T>

typedef polygon2<int>           polygon2i;
typedef polygon2<float>         polygon2f;
typedef polygon2<unsigned int>  polygon2u;

// Initialize invalid with either infinity or max
template<typename T>
T polygon2<T>::invalid = ( polygon2<T>::limit_t::has_infinity ?
                               polygon2<T>::limit_t::infinity( )
                           : // else
                               polygon2<T>::limit_t::max( )
                         );

}  // End namespace euclib

#endif // EUBLIB_POLYGON_HPP

