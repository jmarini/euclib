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
#define GNUPLOT // I am testing this w/ gnuplot, so it should be in that format

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <functional>
#include <time.h>

#include "point.hpp"
#include "direction.hpp"
#include "line.hpp"
#include "segment.hpp"

using namespace euclib;
using namespace std;

// Takes an optional seed as an argument (to recreate bugs)
int main( int argc, char *argv[] ) {
	int seed = time(NULL); // default seed
	if( argc == 2 ) {
		seed = atoi(argv[1]);
	}

	// min and max values when plotting
	int max = 10;

	// random number unif_gen
	uniform_real_distribution<float> unif_distr(0, max);
	normal_distribution<float> norm_distr( max/2, max/10 );
	mt19937 engine( seed );
	auto unif = bind(unif_distr, engine);
	auto norm = bind(norm_distr, engine);

	cout << "seed=" << seed << "\n";

	//////////////////////////////////////////
	//  Testing

	point2i pt1; // default
	point2i pt2 { 1, 3 }; // list, same length
	point2i pt3 { pt2 }; // copy
	point2i pt4 { 2 }; // list, too short
	point3i pt5 { 3, 4, 5 };
	pt1 = pt4; // assignment
	cout << "pt1:   " << pt1.x( ) << "," << pt1.y( ) << "\n";
	cout << "pt2:   " << pt2.x( ) << "," << pt2.y( ) << "\n";
	cout << "pt3:   " << pt3.x( ) << "," << pt3.y( ) << "\n";
	cout << "pt4:   " << pt4.x( ) << "," << pt4.y( ) << "\n";
	cout << "pt5:   " << pt5.x( ) << "," << pt5.y( ) << "," << pt5.z( ) << "\n";
	pt1.x( ) = 4;
	// 2 * [2,0] + [1,3] * 4 + 5 * [1,3] - [2,0]
	pt4 = 2 * pt1 + pt2;// * 4 + 5 * pt2 - pt1; // testing out expression evaluation
	cout << "pt1:   " << pt1.x( ) << "," << pt1.y( ) << "\n";
	cout << "pt4:   " << pt4.x( ) << "," << pt4.y( ) << "\n";
	cout << "pt2 == pt3: " << ( pt2 == pt3 ? "true\n" : "false\n" );
	cout << "pt2 != pt4: " << ( pt2 != pt4 ? "true\n" : "false\n" );

	point<int,10> pt6 { 1, 2, 3, 4, 5, 9 };
	cout << "pt6:   ";
	for( unsigned int i = 0; i < pt6.dimension( ); ++i ) {
		cout << pt6[i] << ( i == pt6.dimension( ) - 1 ? "\n" : "," );
	}

	direction2i dir1 { 3, 4 };
	direction2i dir2 { pt1 };
	direction2i dir3 { 6, 8 };
	direction2i dir4 { dir1 };
	cout << "dir1:  " << dir1[0] << " " << dir1[1] << "\n";
	cout << "dir2:  " << dir2[0] << " " << dir2[1] << "\n";
	cout << "dir3:  " << dir3[0] << " " << dir3[1] << "\n";
	cout << "dir4:  " << dir4[0] << " " << dir4[1] << "\n";
	cout << "dir1 != dir2: " << ( dir1 != dir2 ? "true\n" : "false\n" );
	cout << "dir1 != dir3: " << ( dir1 != dir3 ? "true\n" : "false\n" );
	cout << "dir1 != dir4: " << ( dir1 != dir4 ? "true\n" : "false\n" );

	line2i line1;
	cout << "line1: " << line1.base_point( ).x( ) << " " << line1.base_point( ).y( )
	     << ",  " << line1.base_direction( ).x( ) << " " << line1.base_direction( ).y( ) << "\n";
	line2i line2 { pt2, dir1 };
	line2i line3 { line2 };
	line1 = line3;
	line2i line4 { pt2, pt4 };
	cout << "line1: " << line1.base_point( ).x( ) << " " << line1.base_point( ).y( )
	     << ",  " << line1.base_direction( ).x( ) << " " << line1.base_direction( ).y( ) << "\n";
	cout << "line2: " << line2.base_point( ).x( ) << " " << line2.base_point( ).y( )
	     << ",  " << line2.base_direction( ).x( ) << " " << line2.base_direction( ).y( ) << "\n";
	cout << "line3: " << line3.base_point( ).x( ) << " " << line3.base_point( ).y( )
	     << ",  " << line3.base_direction( ).x( ) << " " << line3.base_direction( ).y( ) << "\n";
	cout << "line4: " << line4.base_point( ).x( ) << " " << line4.base_point( ).y( )
	     << ",  " << line4.base_direction( ).x( ) << " " << line4.base_direction( ).y( ) << "\n";

	segment<float,2,double> seg1 { point2f{ 1.f, 1.f }, point2f{ 2.45f, 5.67f } };
	cout << "seg1:  " << seg1.base_point( ).x( ) << " " << seg1.base_point( ).y( )
	     << ",  " << seg1.base_direction( ).x( ) << " " << seg1.base_direction( ).y( ) << "\n";
	point2f ptf1 = seg1.extrapolate( 1 );
	cout << "pt1:   " << ptf1.x( ) << "," << ptf1.y( ) << "\n";
	ptf1 = seg1.extrapolate( -1 );
	cout << "pt1:   " << ptf1.x( ) << "," << ptf1.y( ) << "\n";

	return 0;
}

