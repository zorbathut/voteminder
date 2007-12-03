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

#include "util.h"

#include "debug.h"

#include <vector>
#include <cmath>
#include <sstream>
#include <stdarg.h>

using namespace std;

bool ffwd = false;

static bool inthread = false;

string StringPrintf(const char *bort, ...) {
  CHECK(!inthread);
  inthread = true;
  
  static vector< char > buf(2);
  va_list args;

  int done = 0;
  bool noresize = false;
  do {
    if(done && !noresize)
      buf.resize(buf.size() * 2);
    va_start(args, bort);
    done = vsnprintf(&(buf[0]), buf.size() - 1,  bort, args);
    if(done >= (int)buf.size()) {
      CHECK(noresize == false);
      CHECK(buf[buf.size() - 2] == 0);
      buf.resize(done + 2);
      done = -1;
      noresize = true;
    } else {
      CHECK(done < (int)buf.size());
    }
    va_end(args);
  } while(done == buf.size() - 1 || done == -1);

  CHECK(done < (int)buf.size());

  string rv = string(buf.begin(), buf.begin() + done);
  
  CHECK(inthread);
  inthread = false;
  
  return rv;
};
