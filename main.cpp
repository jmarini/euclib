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
	    << "set title 'seed = " << seed << "' font 'Arial,12'\n";

	
	// ROTATE / MIRROR SETUP  (point,line,angle)
	point2f about { generator( ), generator( ) };
	line2f over { about, point2f{ generator( ), generator( ) } };
//	float ang = generator( ) * 10.f;
	// POLYGON SETUP  (2 random polys)
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
	// RECT SETUP  (2 random rects)
	rect2f rect1 { point2f{ generator( )/2, generator( )/2 }, generator( )/2, generator( )/2 };	
	rect2f rect2 { point2f{ generator( )/2, generator( )/2 }, generator( )/2, generator( )/2 };	
	// LINE SETUP  (2 random lines)
	line2f line1 { generator( ), generator( ), generator( ), generator( ) };
	line2f line2 { generator( ), generator( ), generator( ), generator( ) };


	int test_num = 2;


	// TEST 1 POLYGON-POINT INTERSECT
	if( test_num == 1 ) {
		point2f loc = overlap( about, poly1 );
		std::cout << "Intersection at " << loc;

		out << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "plot '-' ls 1 with points notitle, "
		    <<      "'-' ls 2 with linespoints notitle, "
		    <<      "'-' ls 3 with linespoints notitle\n";

		out << about << "e\n";
		out << poly1.bounding_box( );
		out << poly1;
	}
	
	// TEST 2 LINE-LINE INTERSECT
	if( test_num == 2 ) {
		point2f loc = overlap( line1, line2 );
		std::cout << "Intersection at " << loc;

		out << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "f" << line1
		    << "g" << line2
		    << "plot f(x) ls 3 with lines notitle, "
		    <<      "g(x) ls 2 with lines notitle, "
		    <<      "'-' ls 1 with linespoints notitle\n";
		
		out << loc << "e\n";
	}
	
	// TEST 3 LINE-RECT INTERSECT
	if( test_num == 3 ) {
		line2f loc = overlap( line1, rect1 );
		std::cout << "Intersection at f" << loc;

		out << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "f" << loc
		    << "g" << line1
		    << "plot f(x) ls 1 with lines notitle, "
		    <<      "g(x) ls 2 with lines notitle, "
		    <<      "'-' ls 3 with linespoints notitle\n";

		out << loc;
		out << line1;
		out << rect1;
	}
	
	// TEST 4 LINE-POLYGON INTERSECT
	if( test_num == 4 ) {
		line2f loc = overlap( line1, poly1 );
		std::cout << "Intersection at f" << loc;

		out << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << loc
		    << "plot f(x) ls 1 with lines notitle, "
		    <<      "'-' ls 2 with linespoints notitle, "
		    <<      "'-' ls 3 with linespoints notitle\n";

		out << poly1.bounding_box( );
		out << poly1;
	}

	// finish up the gnuplot output
	out << "pause -1 'press enter to continue'\n";
	out.close( );

	return 0;
}

