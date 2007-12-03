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

#include "os.h"

#include "debug.h"
#include "util.h"

#include <fstream>
#include <vector>
#include <signal.h>
#include <unistd.h>

using namespace std;

// Cross-platform
static string loc_exename;
void set_exename(const string &str) {
  loc_exename = str;
}

#ifndef NO_WINDOWS

#include <windows.h>

void outputDebugString(const string &str) {
  OutputDebugString(str.c_str());
}

void seriouslyCrash() {
  #ifdef NOEXIT
    TerminateProcess(GetCurrentProcess(), 1); // sigh
  #endif
  exit(-1);
}

#else

#undef printf
void outputDebugString(const string &str) {
  printf("%s\n", str.c_str());
}
#define printf FAILURE

void seriouslyCrash() {
  exit(-1);
}

#endif

// if Cygwin or other Linux
typedef void (*sighandler_t)(int);

class SignalHandler {
public:
  static void signal_h(int signum) {
    disableStackTrace(); // we won't get anything anyway
    CHECK(0);
  }
  
  SignalHandler() {
    signal(SIGSEGV, &signal_h);
  }
} sighandler;

string exename() {
  return loc_exename;
}

// if GCC

#if defined(__GNUG__)
 
const unsigned int kStackTraceMax = 20;
const unsigned int kMaxClassDepth = kStackTraceMax * 2 + 1;
//BOOST_STATIC_ASSERT(kMaxClassDepth % 2 == 1); // Must be odd.
 
template <unsigned int S, unsigned int N = 1> struct StackTracer {
  static void printStack(vector<const void *> *vek) {
    if (!__builtin_frame_address(N))
      return;
 
    if (const void * const p = __builtin_return_address(N)) {
      vek->push_back(p);
      // Because this is recursive(ish), we may have to go down the stack by 2.
      StackTracer<S, N + S>::printStack(vek);
    }
  }
};
 
template <> struct StackTracer<1, kStackTraceMax> {
  static void printStack(vector<const void *> *vek) {}
};

template <> struct StackTracer<2, kMaxClassDepth> {
  static void printStack(vector<const void *> *vek) {}
};

inline bool verifyInlined(const void *const p) {
  return __builtin_return_address(1) == p;
}

bool testInlined() {
  return verifyInlined(__builtin_return_address(1));
}

bool fileexists(const string &fil) {
  FILE *jfil = fopen(fil.c_str(), "r");
  bool rv = jfil;
  if(jfil)
    fclose(jfil);
  return rv;
}
 
void dumpStackTrace() {
  vector<const void *> stack;
  if(testInlined()) {
    dprintf("Stacktracing (inlined)\n");
    StackTracer<1>::printStack(&stack);
  } else {
    dprintf("Stacktracing\n");
    StackTracer<2>::printStack(&stack);
  }
  
  if(fileexists("useaddr2line")) {
    vector<pair<string, string> > tokens;
    {
      string line = "addr2line -f -e " + exename() + " ";
      for(int i = 0; i < stack.size(); i++)
        line += StringPrintf("%p ", stack[i]);
      line += "> addr2linetmp.txt";
      int rv = system(line.c_str());
      if(!rv) {
        {
          ifstream ifs("addr2linetmp.txt");
          string lin;
          while(getline(ifs, lin)) {
            string tlin;
            getline(ifs, tlin);
            tokens.push_back(make_pair(lin, tlin));
          }
        }
        unlink("addr2linetmp.txt");
      } else {
        dprintf("Couldn't call addr2line\n");
        return;
      }
    }
    
    {
      string line = "c++filt -n -s gnu-v3 ";
      for(int i = 0; i < tokens.size(); i++)
        line += tokens[i].first + " ";
      line += "> cppfilttmp.txt";
      int rv = system(line.c_str());
      if(!rv) {
        {
          ifstream ifs("cppfilttmp.txt");
          string lin;
          int ct = 0;
          while(getline(ifs, lin)) {
            if(lin.size() && lin[0] == '_')
              lin.erase(lin.begin());
            dprintf("  %s - %s", tokens[ct].second.c_str(), lin.c_str());
            ct++;
          }
        }
        unlink("cppfilttmp.txt");
      } else {
        dprintf("Couldn't call c++filt\n");
        return;
      }
    }
  } else {
    dprintf("Raw stack dump\n");
    for(int i = 0; i < stack.size(); i++)
      dprintf("%08x\n", (unsigned int)stack[i]);
    dprintf("Raw stack dump done\n");
  }
  
  dprintf("\n");
}

bool isUnoptimized() {
  return !testInlined();
}

#else

void dumpStackTrace() {
  dprintf("No stack trace available\n");
}

bool isUnoptimized() {
  return false;
}

#endif
