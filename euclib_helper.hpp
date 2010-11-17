/*	euclib_helper.hpp  v 0.0.4.10.1117
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

#ifndef EUBLIB_HELPER_HPP
#define EUBLIB_HELPER_HPP

#include "point.hpp"
#include "line.hpp"
#include "segment.hpp"
#include "rect.hpp"
#include "polygon.hpp"

#include <vector>
#include <complex>
// TODO: only for debugging
#include <iostream>
using std::cout;

namespace euclib {

const double PI = 3.1415926536f;

//TODO: angle class??

/**************************
 * Point helper functions *
 **************************/

	template<typename T>
	T dot( const point2<T>& pt1, const point2<T>& pt2 ) {
		return (pt1.x * pt2.x) + (pt1.y * pt2.y);
	}

	template<typename T>
	T cross( const point2<T>& pt1, const point2<T>& pt2 ) {
		return (pt1.x * pt2.y) - (pt1.y * pt2.x);
	}


/*************************
 * Line helper functions *
 *************************/

	

/*************************
 * Translation Functions *
 *************************/

	template<typename T>
	point2<T> translate( const point2<T>& pt, T x, T y ) {
		return point2<T>{ pt.x + x, pt.y + y };
	}
	
	template<typename T>
	line2<T> translate( const line2<T>& line, T x, T y ) {
		return line2<T>{ translate(line.pt1,x,y), translate(line.pt2,x,y) };
	}
	
	template<typename T>
	rect2<T> translate( const rect2<T>& rect, T x, T y ) {
		return rect2<T>{ rect.l + x, rect.r + x, rect.t + y, rect.b + y };
	}
	
	// friend function
	template<typename T>
	polygon2<T> translate( const polygon2<T>& poly, T x, T y ) {
		polygon2<T> new_poly{ poly };
		for( auto itr = new_poly.begin( ); itr != poly.end( ); ++itr ) {
			*itr = translate(*itr, x, y);
		}
		new_poly.m_bounding_box = translate( new_poly.m_bounding_box );
		return new_poly;
	}


/**********************
 * Rotation Functions *
 **********************/
 
 	// TODO: include rotate rect --> poly ??

	template<typename T>
	point2<T> rotate( const point2<T>& target, const point2<T>& about,
	                   float angle, bool clockwise = true ) {
	    // get translation matrix
		float matrix[4] = { (float)cos(angle*PI/180.f), (float)-sin(angle*PI/180.f),
		                     (float)sin(angle*PI/180.f), (float)cos(angle*PI/180.f) };
		if( !clockwise ) {
			matrix[1] *= -1;
			matrix[3] *= -1;
		}

		// translate 'about' to origin
		point2<T> tmp = translate( target, -about.x, -about.y );

		point2f rotated = { (matrix[0]*(float)tmp.x+matrix[1]*(float)tmp.y),
		                    (matrix[2]*(float)tmp.x+matrix[3]*(float)tmp.y) };
		// reduce rounding errors if T is an integer type
		if( std::numeric_limits<T>::is_integer ) {
			rotated.x += 0.5;
			rotated.y += 0.5;
		}

		// translate back
		return translate( point2<T>{ (T)rotated.x, (T)rotated.y }, about.x, about.y );
	}

	template<typename T>
	line2<T> rotate( const line2<T>& target, const point2<T>& about,
	                 float angle, bool clockwise = true ) {
		return line2<T>{ rotate( target.pt1, about, angle, clockwise ),
		                 rotate( target.pt2, about, angle, clockwise ) };
	}

	template<typename T>
	polygon2<T> rotate( const polygon2<T>& target, const point2<T>& about,
	                    float angle, bool clockwise = true ) {
		polygon2<T> poly { target };
		for( auto itr = poly.m_hull.begin( ); itr != poly.m_hull.end( ); ++itr ) {
			*itr = rotate( *itr, about, angle, clockwise );
		}
		return poly;
	}


/********************
 * Mirror Functions *
 ********************/

	template<typename T>
	point2<T> mirror( const point2<T>& target, const line2<T>& over ) {
		// translate point & line to origin
		line2<T> t_over = translate( over, -over.pt1.x, -over.pt1.y );
		point2<T> t_targ = translate( target, -over.pt1.x, -over.pt1.y );
		
		// get translation matrix
		float length = t_over.length( ) * t_over.length( );
		float matrix[4] = {
			((float)t_over.pt2.x*(float)t_over.pt2.x - (float)t_over.pt2.y*(float)t_over.pt2.y),
			2*(float)t_over.pt2.x*(float)t_over.pt2.y,
			2*(float)t_over.pt2.x*(float)t_over.pt2.y,
			((float)t_over.pt2.y*(float)t_over.pt2.y - (float)t_over.pt2.x*(float)t_over.pt2.x)
		};
		if( std::abs( length ) > std::numeric_limits<float>::epsilon( ) ) {
			for( int i = 0; i < 4; ++i ) {
				matrix[i] /= length;
			}
		}

		// calculate new point
		point2f tmp = { matrix[0]*(float)t_targ.x+matrix[1]*(float)t_targ.y,
		                matrix[2]*(float)t_targ.x+matrix[3]*(float)t_targ.y };
		// reduce rounding errors if T is an integer type
		if( std::numeric_limits<T>::is_integer ) {
			tmp.x += 0.5f;
			tmp.y += 0.5f;
		}
		
		// translate back
		return translate( point2<T>{ (T)tmp.x, (T)tmp.y }, over.pt1.x, over.pt1.y );
	}
	
	template<typename T>
	line2<T> mirror( const line2<T>& target, const line2<T>& over ) {
		return line2<T>{ mirror( target.pt1, over ), mirror( target.pt2, over ) };
	}

	template<typename T>
	polygon2<T> mirror( const polygon2<T>& target, const line2<T>& over ) {
		polygon2<T> poly { target };
		for( auto itr = poly.m_hull.begin( ); itr != poly.m_hull.end( ); ++itr ) {
			*itr = mirror( *itr, over );
		}
		return poly;
	}
	

/*********************************
 * Overlap (Intersect) Functions *
 *********************************/
/** overlap ( shape1, shape2 )
 *    intersects two shapes and returns the overlap between them.
    +===========+===========+===========+
    | Shape One | Shape Two |  Result   |
    +===========+===========+===========+
    | Point     | Point     | Point     |
    |           | Line      | Point     |
    |           | Rectangle | Point     |
    |           | Polygon   | Point     |
    +-----------+-----------+-----------+
    | Line      | Line      | Point     |
    |           | Rectangle | Line      |
    |           | Polygon   | Line      |
    +-----------+-----------+-----------+
    | Rectangle | Rectangle | Rectangle |
    |           | Polygon   | Polygon   |
    +-----------+-----------+-----------+
    | Polygon   | Polygon   | Polygon   |
    +-----------+-----------+-----------+
 */

	// TODO: should the lines go on forever??

	// point with *

	template<typename T>
	point2<T> overlap( const point2<T>& pt1, const point2<T>& pt2 ) {
		if( pt1 == pt2 ) {
			return pt1;
		}
		else {
			return point2<T>::null( );
		}
	}
	
	template<typename T>
	point2<T> overlap( const point2<T>& pt, const line2<T>& line ) {
		// check if either is null
		if( pt == point2<T>::null( ) || line == line2<T>::null( ) ) {
			return point2<T>::null( );
		}
		
		// vertical line
		if( line.slope( ) == std::numeric_limits<float>::infinity( ) ) {
			if( std::abs(line.pt1.x -pt.x) <= std::numeric_limits<T>::epsilon( ) ) {
				return pt;
			}
			return point2<T>::null( );
		}
		
		// general line
		float y = line.slope( ) * (float)pt.x + line.intercept( );
		if( std::abs( y - pt.y ) <= std::numeric_limits<float>::epsilon( ) ) {
			return pt;
		}
		else {
			return point2<T>::null( );
		}
	}
	
	template<typename T>
	point2<T> overlap( const point2<T>& pt, const rect2<T>& rect ) {
		// check if either is null
		if( pt == point2<T>::null( ) || rect == rect2<T>::null( ) ) {
			return point2<T>::null( );
		}
		// general case
		else if( pt.x - rect.l > std::numeric_limits<T>::epsilon( ) &&
		         rect.r - pt.x > std::numeric_limits<T>::epsilon( ) &&
		         pt.y - rect.t > std::numeric_limits<T>::epsilon( ) &&
		         rect.b - pt.y > std::numeric_limits<T>::epsilon( ) ) {
			return pt;
		}
		else {
			return point2<T>::null( );
		}
	}
	
	template<typename T>
	point2<T> overlap( const point2<T>& pt, const polygon2<T>& poly ) {
		// check if either is null
		if( pt == point2<T>::null( ) || poly == polygon2<T>::null( ) ) {
			return point2<T>::null( );
		}
		// check bounding box first
		if( overlap( pt, poly.m_bounding_box ) == point2<T>::null( ) ) {
			return point2<T>::null( );
		}
		
		// check actual polygon
		//   the point should be on the same side of every line making
		//   up the polygon if it is inside
		float dir = poly.direction( poly.m_hull[0], poly.m_hull[1], pt );
		bool side = dir > std::numeric_limits<T>::epsilon( );
		for( unsigned int i = 1; i < poly.m_hull.size( ); ++i ) {
			// check last element w/ first
			if( i == poly.m_hull.size( ) - 1 ) {
				dir = poly.direction( poly.m_hull[i], poly.m_hull[0], pt );
			}
			else {
				dir = poly.direction( poly.m_hull[i], poly.m_hull[i+1], pt );
			}
			// different side, can't be inside
			if( side != (dir > std::numeric_limits<T>::epsilon( )) ) {
				return point2<T>::null( );
			}
		}

		return pt;
	}
	
	
	// line with *
	
	template<typename T>
	point2<T> overlap( const line2<T>& line, const point2<T>& pt ) {
		return overlap( pt, line );
	}

	template<typename T>
	point2<T> overlap( const line2<T>& ln1, const line2<T>& ln2 ) {
		// check if null
		if( ln1 == line2<T>::null( ) || ln2 == line2<T>::null( ) ) {
			return point2<T>::null( );
		}
		// check if parallel
		else if( std::abs( ln1.slope( ) - ln2.slope( ) )
		         <= std::numeric_limits<float>::epsilon( ) ) {
			return point2<T>::null( );
		}
		// check if parallel and vertical
		else if( ln1.intercept( ) == std::numeric_limits<float>::infinity( ) &&
		         ln1.intercept( ) == std::numeric_limits<float>::infinity( ) ) {
			return point2<T>::null( );
		}
		
		// if ln1 is vertical
		if( ln1.intercept( ) == std::numeric_limits<float>::infinity( ) ) {
			float x = (float)ln1.pt1.x;
			float y = ln2.slope( )*x + ln2.intercept( );
			// reduce rounding errors if T is an integer type
			if( std::numeric_limits<T>::is_integer ) {
				x += 0.5f;
				y += 0.5f;
			}
			return point2<T> { (T)x, (T)y };
		}
		// if ln2 is vertical
		else if( ln2.intercept( ) == std::numeric_limits<float>::infinity( ) ) {
			float x = (float)ln2.pt1.x;
			float y = ln1.slope( )*x + ln1.intercept( );
			// reduce rounding errors if T is an integer type
			if( std::numeric_limits<T>::is_integer ) {
				x += 0.5f;
				y += 0.5f;
			}
			return point2<T>{ (T)x, (T)y };
		}
		
		// general case
		float x = (ln1.intercept( )-ln2.intercept( )) / (ln2.slope( )-ln1.slope( ));
		float y = ln1.slope( )*x + ln1.intercept( );
		// reduce rounding errors if T is an integer type
		if( std::numeric_limits<T>::is_integer ) {
			x += 0.5f;
			y += 0.5f;
		}
		return point2<T>{ (T)x, (T)y };
	}
	
	// TODO: this seems really messy, there should be a better/cleaner algo
	template<typename T>
	line2<T> overlap( const line2<T>& line, const rect2<T>& rect ) {
		// check null
		if( line == line2<T>::null( ) || rect == rect2<T>::null( ) ) {
			return line2<T>::null( );
		}

		// get intersections with each edge
		point2<T> l = overlap( line, rect.left( ) );
		point2<T> r = overlap( line, rect.right( ) );
		point2<T> t = overlap( line, rect.top( ) );
		point2<T> b = overlap( line, rect.bottom( ) );
		bool i_l, i_r, i_t, i_b;
		i_l = i_r = i_t = i_b = false;

		// which edge does it intersect with?
		if( l.y - rect.t > std::numeric_limits<T>::epsilon( ) &&
		    rect.b - l.y > std::numeric_limits<T>::epsilon( ) ) {
			i_l = true;
		}
		if( r.y - rect.t > std::numeric_limits<T>::epsilon( ) &&
		    rect.b - r.y > std::numeric_limits<T>::epsilon( ) ) {
			i_r = true;
		}
		if( t.x - rect.l > std::numeric_limits<T>::epsilon( ) &&
		    rect.r - t.x > std::numeric_limits<T>::epsilon( ) ) {
			i_t = true;
		}
		if( b.x - rect.l > std::numeric_limits<T>::epsilon( ) &&
		    rect.r - b.x > std::numeric_limits<T>::epsilon( ) ) {
			i_b = true;
		}

		// return proper line
		if( i_l && i_r ) { return line2<T>{ l, r }; }
		else if( i_l && i_t ) { return line2<T>{ l, t }; }
		else if( i_l && i_b ) { return line2<T>{ l, b }; }
		else if( i_r && i_t ) { return line2<T>{ r, t }; }
		else if( i_r && i_b ) { return line2<T>{ r, b }; }
		else if( i_t && i_b ) { return line2<T>{ t, b }; }
		
		return line2<T>::null( );
	}
	
	// TODO: only checks against bounding box right now
	template<typename T>
	line2<T> overlap( const line2<T>& line, const polygon2<T>& poly ) {
		// check null
		if( line == line2<T>::null( ) || poly == polygon2<T>::null( ) ) {
			return line2<T>::null( );
		}
		
		// check bounding box first
		if( overlap( line, poly.m_bounding_box ) == line2<T>::null( ) ) {
			return line2<T>::null( );
		}
		
		return overlap( line, poly.m_bounding_box );
	}


/*************************
 * Combination Functions *
 *************************/
/** combine ( shape1, shape2 )
 *    combines the two shapes into one shape. result will be a complex polygon.
    +===========+===========+===========+
    | Shape One | Shape Two |  Result   |
    +===========+===========+===========+
    | Point     | Polygon   | Polygon   |
    +-----------+-----------+-----------+
    | Line      | Polygon   | Polygon   |
    +-----------+-----------+-----------+
    | Rectangle | Rectangle | Polygon   |
    |           | Polygon   | Polygon   |
    +-----------+-----------+-----------+
    | Polygon   | Polygon   | Polygon   |
    +-----------+-----------+-----------+
 */

//	template<typename T>
	

} // End namespace euclib

#endif // EUBLIB_HELPER_HPP

