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
#ifdef EUCLIB_DECIMAL_TYPES
#	include <decimal/decimal>
#endif

namespace euclib { namespace mpl {

template<typename T>
struct is_decimal {
	enum { value = 0 };
};

#ifdef EUCLIB_DECIMAL_TYPES

template<>
struct is_decimal<std::decimal::decimal32> { enum { value = 1 }; };

template<>
struct is_decimal<std::decimal::decimal64> { enum { value = 1 }; };

template<>
struct is_decimal<std::decimal::decimal128> { enum { value = 1 }; };

#endif

} // End namespace mpl

#ifdef EUCLIB_DECIMAL_TYPES
typedef std::decimal::decimal32 decimal32;
typedef std::decimal::decimal64 decimal64;
typedef std::decimal::decimal128 decimal128;
#endif

} // End namespace euclib

#endif // EUCLIB_TYPE_TRAITS_HPP

