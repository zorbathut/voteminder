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

#ifndef DNET_DEBUG
#define DNET_DEBUG

#include <string>

using namespace std;

/*************
 * CHECK/TEST macros
 */
 
class StackPrinter {
public:
  virtual void Print() const = 0;

  StackPrinter();
  virtual ~StackPrinter();
};

class StackString : public StackPrinter {
public:
  void Print() const;

  StackString(const string &str);

private:
  string str_;
};

void registerCrashFunction(void (*)());
void unregisterCrashFunction(void (*)());
 
void disableStackTrace();

extern void *stackStart;

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

// Almost everything after here is necessary for the vector header patch
#ifdef DPRINTF_MARKUP
int rdprintf(const char *bort, ...) __attribute__((format(printf,1,2)));
#define dprintf(format, args...) rdprintf("%10.10s:%4d: " format, __FILE__, __LINE__, ##args)
#else
int dprintf(const char *bort, ...) __attribute__((format(printf,1,2)));
#endif

extern int frameNumber;

void CrashHandler(const char *fname, int line);
void PrintDebugStack();
void crash_now() __attribute__((__noreturn__));
#define CHECK(x) do { if(__builtin_expect(!(x), 0)) { dprintf("Error at %d, %s:%d - %s\n", frameNumber, __FILE__, __LINE__, #x); CrashHandler(__FILE__, __LINE__); PrintDebugStack(); crash_now(); } } while(0)
// And here would be the end

#define printf FAILURE
 
#endif
