program  := EntropyCoder
packages := unittest++

flags    += -std=c++11 -O2 -g
sanitize ?= undefined leak

################################################################################

sources   := $(shell find src -name *.cpp)
headers   := $(shell find src -name *.h)
objects   := $(patsubst src/%.cpp, build/%.o, $(sources))
tests     := $(filter build/%.test.o,$(objects))
objects   := $(filter-out $(tests) build/main.o build/test.o,$(objects))
clang     := $(findstring clang,${CXX})
flags     += $(if $(packages), $(shell pkg-config --cflags $(packages)))
flags     += -DVERSION=\"$(shell git show --pretty=format:%h -q HEAD)\"
flags     += -DPROGRAM=\"$(program)\" -Isrc -fPIE -flto -Wall -Wextra 
flags     += -Wno-unused-parameter -Werror=return-type -Werror=switch
flags     += -ftemplate-backtrace-limit=0 $(patsubst %,-fsanitize=%,$(sanitize))
flags     += $(if $(clang),-fsanitize-blacklist=sanitize-blacklist.txt,)
libs      += -pie -fuse-ld=gold 
libs      += $(if $(packages), $(shell pkg-config --libs $(packages)))
pch       := $(if $(clang),-include-pch build/pch.h.gch,-include build/pch.h)
all       :  build run-tests
build     :  $(program) test
print-%   :  ; @echo $* = $($*)
.PHONY    :  build run-tests clean
.SECONDARY:
-include    $(patsubst src/%.cpp, build/%.d, $(sources))

################################################################################

build/%.d: src/%.cpp
	@echo "Deps  " $*.cpp
	@mkdir -p $(dir $@)
	@${CXX} $(flags) -MM -MP -MG -MF $@ -MT "build/$*.o $@" $<

build/pch.h:
	@echo "Inc   " pch.h
	@grep -h "#include <" $(filter-out src/test.cpp $(filter %.test.cpp,$(sources) $(headers)),$(sources) $(headers)) | sort | uniq > $@

build/%.h.gch: build/%.h
	@echo "Pch   " $*.h
	@mkdir -p $(dir $@)
	@${CXX} $(flags) -x c++-header $< -o $@

build/%.o: src/%.cpp build/pch.h.gch
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	@${CXX} $(flags) $(pch) -c $< -o $@

$(program): $(objects) build/main.o
	@echo "Link  " $@
	@${CXX} $(flags) $^ $(libs) -o $@

test: $(objects) $(tests) build/test.o
	@echo "Link  " $@
	@${CXX} $(flags) $^ $(libs) -o $@

# Tame ASAN for https://github.com/google/sanitizers/issues/647
run-tests: test
	@echo "Test  " $(program)
	@ASAN_OPTIONS=detect_odr_violation=1 ./$<

clean:
	@echo "Clean  " $(program)
	@rm -Rf build $(program) test
