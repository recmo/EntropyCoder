flags     ?= -O2 -march=native -g
sanitize  ?=
prefix    ?= /usr/local

################################################################################

version   := $(shell git show --pretty=format:%h -q HEAD)
sources   := $(shell find src -name *.cpp)
headers   := $(shell find src -name *.h)
install_h := $(if $(headers),$(shell grep -l VISIBLE $(headers)),)
tests     := src/test.cpp $(shell find src -name *.test.cpp)
sources   := $(filter-out $(tests),$(sources))
objects   := $(patsubst src/%.cpp,build/%.o, $(sources))
gcda      := $(patsubst src/%.cpp,build/%.inst.gcda,$(sources) $(test))
clang     := $(findstring clang,${CXX})
flags     += $(if $(packages), $(shell pkg-config --cflags $(packages)))
flags     += -DVERSION=\"$(version)\" -DNAME=\"$(name)\" -std=c++11 -Isrc -fPIC
flags     += -Wall -Wextra -Wno-unused-parameter -Werror=return-type
flags     += -Werror=switch -ftemplate-backtrace-limit=0 -flto
flags     += -fvisibility-inlines-hidden -fvisibility=hidden
flags     += -DVISIBLE=__attribute__\(\(visibility\(\"default\"\)\)\)
flags     += -DHIDDEN=__attribute__\(\(visibility\(\"hidden\"\)\)\)
inst      := $(patsubst %,-fsanitize=%,$(sanitize))
inst      += $(if $(clang),-fsanitize-blacklist=sanitize-blacklist.txt,)
inst      += $(if $(clang),-fprofile-arcs -ftest-coverage,--coverage)
inst      += $(shell pkg-config --cflags unittest++)
libs      += -fuse-ld=gold
libs      += $(if $(packages), $(shell pkg-config --libs $(packages)))
pch       := $(if $(clang),-include-pch build/pch.h.gch,-include build/pch.h)
all       :  build
build     :  $(if $(install_h),lib$(name).so build/$(name).pc \
             $(patsubst src/%.h,build/%.h,$(install_h)),$(name))
check     :  build/unittest.out ## Run the unit test
coverage  :  coverage/src/index.html ## Measure the code coverage of the units tests
install   :  install-$(name).pc install-lib$(name).so \
             $(patsubst src/%.h,install-%.h,$(install_h))
.PHONY    :  all build check clean coverage install
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

$(name): $(objects)
	@echo "Link  " $(notdir $@)
	@${CXX} $(flags) $^ $(libs) -pie -o $@

lib$(name).so: $(objects)
	@echo "Link  " $(notdir $@)
	@${CXX} $(flags) $^ $(libs) -shared -o $@

unittest: $(patsubst src/%.cpp,build/%.inst.o,$(tests)) \
	$(patsubst %.o,%.inst.o,$(objects)) build/test.inst.o
	@echo "Link  " $@
	@${CXX} $(flags) $(inst) $^ $(libs) \
		$(shell pkg-config --libs unittest++) -o $@

build/unittest.out: unittest
	@echo "Test  " $(name)
	@ASAN_OPTIONS=detect_odr_violation=1 ./$< | tee $@

coverage/src/index.html: build/unittest.out
	@echo "Lcov  " test
	@lcov --quiet --base-directory src --no-external --directory build \
		$(if $(clang),--gcov-tool ./llvm-gcov,) --capture \
		--output-file build/lcov
	@lcov --quiet --remove build/lcov src/*.test.cpp src/test.cpp \
		$(if $(clang),--gcov-tool ./llvm-gcov,) --output-file build/lcov
	@genhtml --quiet --title "$(name)" --legend --num-spaces 4 \
		--output-directory coverage build/lcov

view-coverage: coverage/src/index.html ## View code coverage in the default browser
	@echo "Open  " $^
	@xdg-open $^

build/%.h: src/%.h
	@echo "Sed   " $<
	@cat $< | sed 's/\bVISIBLE //g' | sed 's/\bHIDDEN //g' > $@

build/$(name).pc:
	@echo "Pkgcf " $(notdir $@)
	@echo "Name: $(name)" > $@
	@echo "Description: $(desc)" >> $@
	@echo "URL: $(url)" >> $@
	@echo "Version: $(version)" >> $@
	@echo "Libs: -L$(prefix)/lib -l$(name)" >> $@
	@echo "Cflags: -I$(prefix)/include/$(name)" >> $@

install-%.so: %.so
	@echo "Inst  " $(prefix)/lib/$(notdir $<)
	@install -C $< $(prefix)/lib/$(notdir $<)
	@ldconfig

install-%.h: build/%.h
	@echo "Inst  " $(prefix)/include/$(name)/$(notdir $<)
	@install -C -D $< $(prefix)/include/$(name)/$(notdir $<)

install-%.pc: build/%.pc
	@echo "Inst  " $(prefix)/lib/pkgconfig/$(notdir $<)
	@install -C -D $< $(prefix)/lib/pkgconfig/$(notdir $<)

install-%.py: src/%.py
	@echo "Inst  " $(prefix)/lib/pkgconfig/$(notdir $<)
	@install -C -D $< $(prefix)/lib/pkgconfig/$(notdir $<)

clean: ## Remove all generated files
	@echo "Clean  " $(name)
	@rm -Rf build coverage unittest $(name) lib$(name).so

print-%: ## Print the value of variable % (for debugging Makefiles)
	@echo $* = $($*)

# http://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
help: ## Show makefile help
	@grep -hE '^[a-zA-Z_-% ]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
