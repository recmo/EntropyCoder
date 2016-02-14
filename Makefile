name      := EntropyCoder
packages  := unittest++

flags     ?= -O2 -march=native -g
sanitize  ?= undefined leak
prefix    ?= /usr/local

################################################################################

sources   := $(shell find src -name *.cpp ! -name *.test.cpp)
headers   := $(shell find src -name *.h)
tests     := $(shell find src -name *.test.cpp)
objects   := $(patsubst src/%.cpp,build/%.o, $(sources))
gcda      := $(patsubst src/%.cpp,build/%.inst.gcda,$(sources) $(test))
clang     := $(findstring clang,${CXX})
flags     += $(if $(packages), $(shell pkg-config --cflags $(packages)))
flags     += -DVERSION=\"$(shell git show --pretty=format:%h -q HEAD)\"
flags     += -DNAME=\"$(name)\" -std=c++11 -Isrc -fPIE -Wall -Wextra 
flags     += -Wno-unused-parameter -Werror=return-type -Werror=switch
flags     += -ftemplate-backtrace-limit=0 -flto
flags     += -fvisibility-inlines-hidden -fvisibility=hidden
flags     += -DEXPORT=__attribute__\(\(visibility\(\"default\"\)\)\)
inst      := $(patsubst %,-fsanitize=%,$(sanitize))
inst      += $(if $(clang),-fsanitize-blacklist=sanitize-blacklist.txt,)
inst      += $(if $(clang),-fprofile-arcs -ftest-coverage,--coverage)
libs      += -pie -fuse-ld=gold
libs      += $(if $(packages), $(shell pkg-config --libs $(packages)))
pch       := $(if $(clang),-include-pch build/pch.h.gch,-include build/pch.h)
all       :  build run-tests
build     :  lib$(name).so test
coverage  :  coverage/src/index.html
run-tests :  build/test
print-%   :  ; @echo $* = $($*)
.PHONY    :  build run-tests clean coverage
.SECONDARY:
-include    $(patsubst src/%.cpp, build/%.d, $(sources))

################################################################################

build/%.d: src/%.cpp
	@echo "Deps  " $*.cpp
	@mkdir -p $(dir $@)
	@${CXX} $(flags) -MM -MP -MG -MF $@ -MT "build/$*.o $@" $<

build/pch.h:
	@echo "Inc   " pch.h
	@grep -h "#include <" $(filter-out src/test.cpp %.test.cpp,\
		$(sources) $(headers)) | sort | uniq > $@

build/%.h.gch: build/%.h
	@echo "Pch   " $*.h
	@mkdir -p $(dir $@)
	@${CXX} $(filter-out -DVERSION=%,$(flags)) -x c++-header $< -o $@

build/%.o: src/%.cpp build/pch.h.gch
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	@${CXX} $(flags) $(pch) -c $< -o $@

build/%.inst.o: src/%.cpp build/pch.h.gch
	@echo "Inst  " $*.cpp
	@mkdir -p $(dir $@)
	@${CXX} $(flags) $(pch) $(inst) -c $< -o $@

lib$(name).so: $(objects)
	@echo "Link  " $@
	@${CXX} $(flags) $^ $(libs) -shared -o $@

test: $(patsubst src/%.cpp,build/%.inst.o,$(tests)) \
	$(patsubst %.o,%.inst.o,$(objects)) build/test.inst.o
	@echo "Link  " $@
	@${CXX} $(flags) $(inst) $^ $(libs) -o $@

build/test: test
	@echo "Test  " $(name)
	@ASAN_OPTIONS=detect_odr_violation=1 ./$<
	@touch $<

coverage/src/index.html: build/test
	@echo "Lcov  " test
	@lcov --quiet --base-directory src --no-external --directory build \
		$(if $(clang),--gcov-tool ./llvm-gcov,) --capture \
		--output-file build/lcov
	@lcov --quiet --remove build/lcov src/*.test.cpp src/test.cpp \
		$(if $(clang),--gcov-tool ./llvm-gcov,) --output-file build/lcov
	@genhtml --quiet --title "$(name)" --legend --num-spaces 4 \
		--output-directory coverage build/lcov

view-coverage: coverage/src/index.html
	@echo "Open  " $^
	@xdg-open $^

clean:
	@echo "Clean  " $(name)
	@rm -Rf build coverage lib$(name).so test
