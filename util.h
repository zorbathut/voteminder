/*

Mafia Voteminder
Copyright (C) 2007 Ben Wilhelm

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, Version 3 only.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

I can be contacted at zorba-foreman@pavlovian.net

*/

#ifndef DNET_UTIL
#define DNET_UTIL

#include <string>
#include <vector>

using namespace std;

extern bool ffwd;

/*************
 * Text processing
 */

#ifdef printf
#define PFDEFINED
#undef printf
#endif

string StringPrintf(const char *bort, ...) __attribute__((format(printf,1,2)));

#ifdef PFDEFINED
#define printf FAILURE
#undef PFDEFINED
#endif

namespace detail {
  template <typename type, ::std::size_t size> char (&array_size_impl(type(&)[size]))[size];
}
#define ARRAY_SIZE(array) sizeof(::detail::array_size_impl(array))

#endif
