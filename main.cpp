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
//	int min = -1;
	
	// random number unif_gen
	uniform_real_distribution<float> unif(0, max);
	normal_distribution<float> norm( max/2, max/10 );
	mt19937 engine( seed );
	auto unif_gen = bind(unif, engine);
	auto norm_gen = bind(norm, engine);

	// set up gnuplot
	ofstream out( "plot.out" );
	out << "set xrange [" << max*-1.5 << ":" << max*1.5 << "]\n"
	    << "set yrange [" << max*-1.5 << ":" << max*1.5 << "]\n"
	    << "unset mouse\nset size square\n";
	// seed in title to recreate results
	    
	// POINT SETUP (1 random point,origin)
	point2f pt1 { unif_gen( ), unif_gen( ) };
	point2f origin { 0.f, 0.f };
	// ROTATE / MIRROR SETUP  (point,line,angle)
	point2f about { unif_gen( ), unif_gen( ) };
	line2f over { about, point2f{ unif_gen( ), unif_gen( ) } };
	float ang = unif_gen( ) * 18.f;
	// POLYGON SETUP  (2 random polys)
	int num_points = 20;
	int num_points_norm = 1000;
	polygon2f poly1;
	for( int i = 0; i < num_points; i++ ) {
		point2f pt { unif_gen( ), unif_gen( ) };
		poly1.add_points( pt );
	}
	polygon2f poly2;
	vector<point2f> vec;
	vec.reserve( num_points_norm );
	for( int i = 0; i < num_points_norm; i++ ) {
		point2f pt { norm_gen( ), norm_gen( ) };
		vec.push_back( pt );
	}
	poly2.add_points( vec );
	// RECT SETUP  (2 random rects)
	rect2f rect1 { point2f{ unif_gen( )/2, unif_gen( )/2 }, unif_gen( )/2, unif_gen( )/2 };	
	rect2f rect2 { point2f{ unif_gen( )/2, unif_gen( )/2 }, unif_gen( )/2, unif_gen( )/2 };	
	// LINE SETUP  (2 random lines)
	segment2f seg1 { unif_gen( ), unif_gen( ), unif_gen( ), unif_gen( ) };
	segment2f seg2 { unif_gen( ), unif_gen( ), unif_gen( ), unif_gen( ) };
	line2f line1 = make_line( seg1 );
	line2f line2 = make_line( seg2 );
	


	int test_num = 0;


	// TEST 0 POLYGON-POINT INTERSECT
	if( test_num == 0 ) {
		point2f loc = overlap( about, poly1 );
		std::cout << "Intersection at " << loc;

		out << "set title 'seed = " << seed << " polygon-point intersection'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "plot '-' ls 1 with points notitle, "
		    <<      "'-' ls 2 with linespoints notitle, "
		    <<      "'-' ls 3 with linespoints notitle\n";

		out << about << "e\n";
		out << poly1.bounding_box( );
		out << poly1;
	}
	
	// TEST 1 LINE TRANSLATE
	if( test_num == 1 ) {
		line2f loc = translate( over, about.x/2, about.y/2 );

		out << "set title 'seed = " << seed << "\tx = " << about.x/2 << ", y = " << about.y/2 << "'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "f" << loc
		    << "g" << over
		    << "plot '-' ls 1 with linespoints notitle, "
		    << "f(x), g(x)\n";
		
		out << rect2f{ about, about.x/2, about.y/2 };
	}
	
	// TEST 2 SEGMENT/POINT TRANSLATE
	if( test_num == 2 ) {
		segment2f loc = translate( seg1, about.x/2, about.y/2 );

		out << "set title 'seed = " << seed << "\tx = " << about.x/2 << ", y = " << about.y/2 << "'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "plot '-' ls 1 with linespoints notitle, "
		    <<      "'-' ls 2 with linespoints notitle, "
		    <<      "'-' ls 3 with linespoints notitle\n";

		out << rect2f{ seg1.pt1, about.x/2, about.y/2 };
		out << seg1;
		out << loc;
	}
	
	// TEST 3 POLYGON TRANSLATE
	if( test_num == 3 ) {
		polygon2f loc = translate( poly1, about.x/2, about.y/2 );

		out << "set title 'seed = " << seed << "\tx = " << about.x/2 << ", y = " << about.y/2 << "'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "plot '-' ls 1 with linespoints notitle, "
		    <<      "'-' ls 2 with linespoints notitle, "
		    <<      "'-' ls 3 with linespoints notitle\n";

		out << rect2f{ poly1[0], about.x/2, about.y/2 };
		out << poly1;
		out << loc;
	}
	
	// TEST 4 SEGMENT/POINT ROTATE
	if( test_num == 4 ) {
		segment2f loc = rotate( seg1, about, ang );
		segment2f circ { about, seg1.pt1 };
		segment2f circ2 { about, seg1.pt2 };

		out << "set parametric\n"
		    << "set title 'seed = " << seed << "\tangle = " << ang << "'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "plot [0:2*pi] " << circ.length( ) << "*sin(t)+" << about.x << ","<< circ.length( ) << "*cos(t)+" << about.y << " ls 1 notitle,"
		    <<  circ2.length( ) << "*sin(t)+" << about.x << ","<< circ2.length( ) << "*cos(t)+" << about.y << " ls 1 notitle,"
		    << "'-' ls 1 with points notitle, "
		    << "'-' ls 2 with linespoints notitle, "
		    << "'-' ls 3 with linespoints notitle\n";		
		
		out << origin << about << "e\n";
		out << seg1;
		out << loc;
	}
	
	// TEST 5 POLYGON ROTATE
	if( test_num == 5 ) {
		polygon2f loc = rotate( poly1, about, ang );
		segment2f circ { about, poly1[static_cast<unsigned int>(poly1.size( )/2)] };

		out << "set parametric\n"
		    << "set title 'seed = " << seed << "\tangle = " << ang << "'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "plot [0:2*pi] " << circ.length( ) << "*sin(t)+" << about.x << ","<< circ.length( ) << "*cos(t)+" << about.y << " ls 1 notitle,"
		    << "'-' ls 1 with points notitle, "
		    << "'-' ls 2 with linespoints notitle, "
		    << "'-' ls 3 with linespoints notitle\n";
		
		out << origin << about << "e\n";
		out << poly1;
		out << loc;
	}
	
	// TEST 6 POINT/SEGMENT MIRROR
	if( test_num == 6 ) {
		segment2f loc = mirror( seg1, over );
		
		out << "set title 'seed = " << seed << "'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "f" << over
		    << "plot f(x) ls 1 notitle,"
		    << "'-' ls 1 with points notitle, "
		    << "'-' ls 2 with linespoints notitle, "
		    << "'-' ls 3 with linespoints notitle\n";

		out << origin << "e\n";
		out << seg1;
		out << loc;
	}

	// TEST 7 POLYGON MIRROR
	if( test_num == 7 ) {
		polygon2f loc = mirror( poly1, over );
		
		out << "set title 'seed = " << seed << "'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "f" << over
		    << "plot f(x) ls 1 notitle,"
		    << "'-' ls 1 with points notitle, "
		    << "'-' ls 2 with linespoints notitle, "
		    << "'-' ls 3 with linespoints notitle\n";

		out << origin << "e\n";
		out << poly1;
		out << loc;
	}
	
	// TEST 8 LINE-LINE INTERSECT
	if( test_num == 8 ) {
		point2f loc = overlap( pt1, seg1 );
		
		out << "set title 'seed = " << seed << "'"
		    << "font 'Arial,12'\n"
		    << "set style line 1 pointtype 7 linecolor rgb 'black'\n"
		    << "set style line 2 pointtype 7 linecolor rgb 'red'\n"
		    << "set style line 3 pointtype 7 linecolor rgb 'green'\n"
		    << "plot '-' ls 1 with points notitle, "
		    << "'-' ls 2 with linespoints notitle, "
		    << "'-' ls 3 with linespoints notitle\n";

		out << origin << "e\n";
		out << seg1;
		out << pt1 << "e\n";
		cout << "Intersection at " << loc;
	}

	// finish up the gnuplot output
	out << "pause -1 'press enter to continue'\n";
	out.close( );

	return 0;
}

