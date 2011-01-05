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

#ifndef EUBLIB_ANGLE_HPP
#define EUBLIB_ANGLE_HPP

#include "euclib_math.hpp"

class angle {
// Variables
private:

	double m_radians;


// Constructors
public:

	angle( ) : m_radians( 0 ) { }
	angle( double radians ) : m_radians( radians ) { clamp( ); }
	angle( double degrees ) : m_degrees( degrees*EUCLIB_PI_180 ) { clamp( ); }


// Methods
public:

	double radians( ) const { return m_radians; }
	double degrees( ) const { return m_radians * EUCLIB_180_PI; }

	void set( double radians ) {
		m_radians = radians;
		clamp( );
	}
	void set( double degrees ) {
		m_radians = degrees * EUCLIB_PI_180;
		clamp( );
	}

	void add( double radians ) {
		m_radians += radians;
		clamp( );
	}
	void add( double degrees ) {
		m_radians += degrees * EUCLIB_PI_180;
		clamp( );
	}

	void negate( ) {
		m_radians *= -1;
		clamp( );
	}


private:

	// clamp value to be from 0 to 2pi
	void clamp( ) {
		while( less_than( m_radians, 0 ) ) {
			m_radians += EUCLIB_2_PI;
		}
		while( greater_equal( m_radians, EUCLIB_2_PI ) ) {
			m_radians -= EUCLIB_2_PI;
		}
	}

}; // End class angle

}  // End namespace euclib

#endif // EUBLIB_ANGLE_HPP
