program := EntropyCoder
packages := unittest++

version := $(shell git show --pretty=format:%h -q HEAD)
sources := $(shell find -wholename './src/*.cpp')
headers := $(shell find -wholename './src/*.h')
resources := $(shell find -wholename './src/*.qrc')

# sanitizers ?= -fsanitize=undefined,safe-stack,cfi
# https://llvm.org/bugs/show_bug.cgi?id=22757
sanitizers :=

# C++14 Compiler
CXX := clang++-3.8
flags := -std=c++14 -O3 -fPIC -g -Isrc
flags += -Wall -Wextra -Wno-unused-parameter -Werror=return-type
flags += -Werror=switch -Wdocumentation
flags += -ftemplate-backtrace-limit=0
flags += $(if ${packages}, $(shell pkg-config --cflags ${packages}))
defines := -DPROGRAM=\"$(program)\" -DVERSION=\"$(version)\"
precompiled := -include-pch build/precompiled.pch
lto := -flto

# Package config flags
libs := $(if ${packages}, $(shell pkg-config --libs ${packages}))

precompile := ${CXX} $(flags)
compile := ${CXX} $(flags) $(sanitizers) $(lto) $(precompiled) $(defines)
link := ${CXX} $(flags) -fuse-ld=gold $(sanitizers) $(lto) $(libs)
tidy := clang-tidy -checks='*,-clang-analyzer-alpha.deadcode.UnreachableCode'

# All object files
objects := $(patsubst ./src/%.cpp, ./build/%.o, $(sources))
objects += $(patsubst ./src/%.qrc, ./build/%.o, $(resources))
test_objects := $(filter ./build/%.test.o,$(objects))
objects := $(filter-out $(test_objects) ./build/main.o ./build/test.o,$(objects))

# Run the test and make the whole program by default
all: build run-tests

# Keep all intermediates
.SECONDARY:

# Targets not associated with files
.PHONY: build run-tests clean

# Include header dependency info
-include $(patsubst ./src/%.cpp, ./build/%.d, $(sources))

build: $(program) test

# Build header dependency info
build/%.d: src/%.cpp
	@echo "Deps  " $*.cpp
	@mkdir -p $(dir $@)
	@${CXX} $(flags) -MM -MP -MG -MF $@ -MT "build/$*.o $@" $<

build/precompiled.h:
	@echo "Inc   " precompiled.h
	@grep -h "#include <" $(sources) $(headers) | sort | uniq > $@

build/%.pch: build/%.h
	@echo "Pch   " $*.h
	@mkdir -p $(dir $@)
	@$(precompile) -x c++-header $< -o $@

build/%.o: src/%.cpp build/precompiled.pch
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	@$(compile) -c $< -o $@

$(program): $(objects) build/main.o
	@echo "Link  " $@
	@$(link) $^ -o $@

tidy: build/tidy
build/tidy: $(sources)
	@echo "Tidy  " $(program)
	@$(tidy) $^ -- $(flags) $(defines) 2> /dev/null
	@touch build/tidy

test: $(objects) $(test_objects) build/test.o
	@echo "Link  " $@
	@$(link) $^ $(libs) -o $@

run-tests: test
	@echo "Test  " $(program)
	@./$<

clean:
	@echo "Clean  " $(program)
	@rm -Rf build $(program) test
