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

#ifndef DNET_OS
#define DNET_OS

#include <string>

using namespace std;

// Debug stuff
void outputDebugString(const string &str);

void dumpStackTrace();

bool isUnoptimized();

// This can be used by stack traces and such
void set_exename(const string &str); // this should be removed eventually I think

// OS stuff
void seriouslyCrash() __attribute__((__noreturn__)); // apparently this is needed. Why? Because SDL is stupid.

#endif
