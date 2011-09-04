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
#include <ctime>
#include <memory>
#include <typeinfo>

#include "vector.hpp"

using namespace euclib;
using namespace std;


// Takes an optional seed as an argument (to recreate bugs)
int main( int argc, char *argv[] ) {
	int seed = static_cast<int>( time( NULL ) ); // default seed
	if( argc == 2 ) {
		seed = atoi(argv[1]);
	}

	// min and max values when plotting
	float max = 10.f;

	// random number unif_gen
	uniform_real_distribution<float> unif_distr(0.f, max);
	normal_distribution<float> norm_distr( max/2.f, max/10.f );
	mt19937 engine( seed );
	//auto unif = bind(unif_distr, engine);
	//auto norm = bind(norm_distr, engine);

	cout << "seed=" << seed << "\n";

	//////////////////////////////////////////
	//  Testing

	////////////////////////
	// vector<T,N>
	typedef euclib::vector<float,6> vec6f;
	#define PRINT(v) std::for_each(v.begin(),v.end(),[](float f){std::cout<<f<<" ";});std::cout<<"\n";

	// Constructors
	vec6f v6_1;												// Default constructor does nothing
	vec6f v6_2( 1,2,3,4,5,6 );								// Init list constructor
		assert( v6_2[3] == 4 );
	vec6f v6_3{ 1,2,3,4,5 };								// Init list constructor
		assert( v6_3[5] == 0 );
	vec6f v6_4( v6_2 );										// Copy constructor
		assert( std::mismatch( v6_4.begin( ), v6_4.end( ), v6_2.begin( ) )
				== std::make_pair( v6_4.end( ), v6_2.end( ) ) );
		assert( v6_4 == v6_2 );								// Equality operator
	vec6f v6_5( std::move( v6_4 ) );						// Move constructor
		assert( v6_5 == v6_2 );
	std::array<float,6> arr1 = {{ 6,5,4,3,2,1 }};
	vec6f v6_6( arr1 );										// std::array copy constructor
		assert( std::mismatch( arr1.begin( ), arr1.end( ), v6_6.begin( ) )
				== std::make_pair( arr1.end( ), v6_6.end( ) ) );
	std::array<float,6> arr2 = {{ 1,3,5,7,9,11 }};
	vec6f v6_7( std::move(arr2) );							// std::array move constructor
		assert( v6_7[3] == 7 );
//	vec6f v6_8( 1,2,3,4,5,6,7,8 );							// error (as it should be)

	// Vector Methods
	assert( v6_6.length_sq( ) == 91.f );
	assert( v6_6.length( ) == std::sqrt( 91.f ) );
	assert( v6_6.normalize( ) == vec6f( 6.f/sqrt(91.f), 5.f/sqrt(91.f), 4.f/sqrt(91.f), 3.f/sqrt(91.f),2.f/sqrt(91.f),1.f/sqrt(91.f) ) ); 
	v6_6.normalize_ip( );
	assert( v6_6 == vec6f( 6.f/sqrt(91.f), 5.f/sqrt(91.f), 4.f/sqrt(91.f), 3.f/sqrt(91.f),2.f/sqrt(91.f),1.f/sqrt(91.f) ) );
	
	// Iterator Methods
	std::sort( v6_6.begin( ), v6_6.end( ) );
	assert( v6_6 == vec6f( 1.f/sqrt(91.f), 2.f/sqrt(91.f), 3.f/sqrt(91.f), 4.f/sqrt(91.f), 5.f/sqrt(91.f), 6.f/sqrt(91.f) ) );
	std::sort( v6_6.rbegin( ), v6_6.rend( ) );
	assert( v6_6 == vec6f( 6.f/sqrt(91.f), 5.f/sqrt(91.f), 4.f/sqrt(91.f), 3.f/sqrt(91.f), 2.f/sqrt(91.f), 1.f/sqrt(91.f) ) );

	// Misc Methods
	v6_1.fill( 34.f );
	vec6f( 2,4,6,8,10,12 ).swap( v6_4 );
	for( std::size_t i = 0; i < v6_7.size( ); ++i ) {
		v6_7[i] = i;
	}
	assert( v6_7 == vec6f( 0,1,2,3,4,5 ) );


	PRINT(v6_1);
	PRINT(v6_2);
	PRINT(v6_3);
	PRINT(v6_4);
	PRINT(v6_5);
	PRINT(v6_6);
	PRINT(v6_7);	


	return 0;
}

