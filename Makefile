CXX ?= g++
CPPFLAGS += -Isrc
CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra -Wpedantic
ifeq ($(OS),Windows_NT)
WINCXX ?= g++
WINAR ?= ar
else
WINCXX ?= x86_64-w64-mingw32-g++
WINAR ?= x86_64-w64-mingw32-ar
endif
BGI_SRC := third_party/winbgim
BGI_NAMES := drawing misc mouse palette text winbgi winthread file dibutil
BGI_OBJECTS := $(addprefix build/winbgim/,$(addsuffix .o,$(BGI_NAMES)))

.PHONY: all windows run check clean
all: windows
windows: build/airport.exe

build build/winbgim:
	mkdir -p $@

build/winbgim/%.o: $(BGI_SRC)/%.cxx $(wildcard $(BGI_SRC)/*.h) | build/winbgim
	$(WINCXX) -O2 -Wno-write-strings -I$(BGI_SRC) -c $< -o $@

build/winbgim/libbgi.a: $(BGI_OBJECTS)
	$(WINAR) rcs $@ $^

build/airport.exe: src/main.cpp src/simulation.h src/display.h build/winbgim/libbgi.a | build
	$(WINCXX) $(CPPFLAGS) $(CXXFLAGS) -I$(BGI_SRC) src/main.cpp -Lbuild/winbgim -static -o $@ -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32

ifeq ($(OS),Windows_NT)
run: windows
	./build/airport.exe
else
run: windows
	wine build/airport.exe
endif

build/simulation_test: tests/simulation_test.cpp src/simulation.h | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/simulation_test.cpp -o $@

check: build/simulation_test
	./build/simulation_test
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Itests/support -fsyntax-only src/main.cpp

clean:
	rm -rf build
