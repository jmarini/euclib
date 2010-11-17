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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <functional>

#include "euclib.hpp" // includes all other files needed from euclib

using namespace euclib;
using namespace std;

// Takes an optional seed as an argument (to recreate bugs)
int main( int argc, char *argv[] ) {
	int seed = time(NULL); // default seed
	if( argc == 2 ) {
		seed = atoi(argv[1]);
	}
	
	// min and max values when plottings
	int max = 10;
	int min = -1;
	
	// random number generator
	uniform_real_distribution<float> distr(0, max);
	mt19937 engine( seed );
	auto generator = bind(distr, engine);

	// set up gnuplot
	ofstream out( "plot.out" );
	out << "set xrange [" << min << ":" << max+1 << "]\n"
	    << "set yrange [" << min << ":" << max+1 << "]\n"
	    << "unset mouse\nset size square\n"
	// seed in title to recreate results
	    << "set title 'seed = " << seed << "' font 'Arial,12'\n"
	// the different styles
	    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
	    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
	    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
	// change depending on if you want lines or points
	    << "plot '-' ls 1 with linespoints notitle, '-' ls 2 with linespoints notitle, '-' ls 3 with linespoints notitle\n";

	
	// ROTATE / MIRROR SETUP
	point2f about { generator( ), generator( ) };
	line2f over { about, point2f{ generator( ), generator( ) } };
//	float ang = generator( ) * 10.f;

	// POLYGON SETUP
	int num_points = 10;
	polygon2f poly1;
	for( int i = 0; i < num_points; i++ ) {
		point2f pt { generator( ), generator( ) };
		poly1.add_points( pt );
	}
	polygon2f poly2;
	for( int i = 0; i < num_points; i++ ) {
		point2f pt { generator( ), generator( ) };
		poly2.add_points( pt );
	}
	
	// RECT SETUP
	rect2f rect1 { point2f{ generator( )/2, generator( )/2 }, generator( )/2, generator( )/2 };	
	rect2f rect2 { point2f{ generator( )/2, generator( )/2 }, generator( )/2, generator( )/2 };	
	
	// LINE SETUP
	line2f line1 { generator( ), generator( ), generator( ), generator( ) };
	line2f line2 { generator( ), generator( ), generator( ), generator( ) };
	
	int test_num = 1;

	// TEST 1 POLYGON-POINT INTERSECT
	if( test_num == 1 ) {
		point2f loc = overlap( about, poly1 );
		loc.print( cout, true );
		
		about.gnuplot( out ); out << "e\n";
		poly1.bounding_box( ).gnuplot( out );
		poly1.gnuplot( out );
	}
	
	// TEST 2 LINE-LINE INTERSECT
	if( test_num == 2 ) {
		point2f loc = overlap( line1, line2 );
		loc.print( cout, true );
		
		loc.gnuplot( out ); out << "e\n";
		line1.gnuplot( out );
		line2.gnuplot( out );
	}
	
	// TEST 3 LINE-RECT INTERSECT
	if( test_num == 3 ) {
		line2f loc = overlap( line1, rect1 );
		loc.print( cout, true );
		
		loc.gnuplot(out  );
		line1.gnuplot( out );
		rect1.gnuplot( out );
	}
	
	// TEST 4 LINE-POLYGON INTERSECT
	if( test_num == 4 ) {
		line2f loc = overlap( line1, poly1 );
		loc.print( cout, true );
		
		loc.gnuplot( out );
		poly1.bounding_box( ).gnuplot( out );
		poly1.gnuplot( out );
	}

	// finish up the gnuplot output
	out << "pause -1 'Press enter to exit'\n";
	out.close( );

	return 0;
}

