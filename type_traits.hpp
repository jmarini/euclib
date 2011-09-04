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

#ifndef EUBLIB_TYPE_TRAITS_HPP
#define EUBLIB_TYPE_TRAITS_HPP

#include <type_traits>

namespace euclib { namespace mpl {

struct inaccurate_tag { };
struct accurate_tag { };

template<typename T>
struct accuracy_traits { typedef accurate_tag category_type; };

template< >
struct accuracy_traits<float> { typedef inaccurate_tag category_type; };
template< >
struct accuracy_traits<double> { typedef inaccurate_tag category_type; };
template< >
struct accuracy_traits<long double> { typedef inaccurate_tag category_type; };

} // End namespace mpl

} // End namespace euclib

#endif // EUCLIB_TYPE_TRAITS_HPP

