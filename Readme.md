# EntropyCoder [![Build Status](https://travis-ci.org/Recmo/EntropyCoder.svg)](https://travis-ci.org/Recmo/EntropyCoder) [![Documentation Status](https://readthedocs.org/projects/entropycoder/badge/?version=latest)](http://entropycoder.readthedocs.org/en/latest/?badge=latest)

<!-- https://landscape.io/ -->

<!-- http://shields.io/ -->

Library for optimal compression using entropy/arithmetic/range coding.

## Getting up and running

**Configuring your system**:

	apt-get install make git clang libunittest++-dev

Make sure clang supports `-std=c++14`. The following should not produce errors:

	echo "int main(){return 0;}" | clang++ -std=c++14 -x c++ - -o /dev/null

**Obtaining the project source code**:

	git clone git@github.com:Recmo/EntropyCoder.git
	cd EntropyCoder

**Building the project source code**:

	make build

**Testing the project build**:

	make run-tests

## Making changes


**Submitting changes**:

*To be written*

## Appendix

### Building and running in docker

	cd EntropyCoder
	docker run -t -i -v (pwd)/.git:/git:ro ubuntu:wily /bin/bash

Then in the docker instance:

	apt-get update
	apt-get install -y git make clang pkg-config libunittest++-dev

	git clone /git /build
	cd /build
	make
