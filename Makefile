
STDCXXFLAGS :=  -DVECTOR_PARANOIA -I/usr/mingw/local/include -I/usr/mingw/local/include/boost-1_33_1 -mno-cygwin -DDPRINTF_MARKUP -Wall -Wno-sign-compare -Wno-uninitialized -g
STDLDFLAGS := -L/usr/mingw/local/lib -mno-cygwin -lboost_regex-mgw-mt -lboost_filesystem-mgw-mt -lmingw32 -g

VOTEMINDERSOURCES := main debug parse util os
CONSOLECXXFLAGS := $(STDCXXFLAGS) -O0
CONSOLELDFLAGS := $(STDLDFLAGS) -O0

CXX := g++
CC := gcc

all: voteminder

include $(patsubst %,build/%.c.d,$(VOTEMINDERSOURCES))

voteminder: voteminder.exe

voteminder.exe: $(patsubst %,build/%.c.o,$(VOTEMINDERSOURCES))
	nice $(CXX) -o $@ $(patsubst %,build/%.c.o,$(VOTEMINDERSOURCES)) $(CONSOLELDFLAGS)

clean:
	rm -rf build/*.o *.exe build/*.d build/*.S

run: voteminder
	./voteminder.exe

package: voteminder
	rm -rf deploy
	rm -rf voteminder.zip
	mkdir deploy
	cp voteminder.exe deploy
	strip -s deploy/*.exe deploy/*.dll
	cd deploy ; zip -9 -r ../voteminder.zip *
	rm -rf deploy

build/%.c.o: %.cpp
	nice $(CXX) $(CONSOLECXXFLAGS) -c -o $@ $<

build/%.c.S: %.cpp
	nice $(CXX) $(CONSOLECXXFLAGS) -c -Wa,-a,-ad $< > $@

build/%.c.d: %.cpp
	nice bash -ec '$(CXX) $(CONSOLECXXFLAGS) -MM $< | sed "s!$*.o!build/$*.c.o build/$*.c.d.o $@!g" > $@'

build/%.c.d: %.c
	nice bash -ec '$(CXX) $(CONSOLECXXFLAGS) -MM $< | sed "s!$*.o!build/$*.c.o build/$*.c.d.o $@!g" > $@'
