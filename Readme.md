# EntropyCoder [![Build Status](https://travis-ci.org/Recmo/EntropyCoder.svg)](https://travis-ci.org/Recmo/EntropyCoder) [![Documentation Status](https://readthedocs.org/projects/entropycoder/badge/?version=latest)](http://entropycoder.readthedocs.org/en/latest/?badge=latest)

<!-- https://landscape.io/ -->

<!-- http://shields.io/ -->

Library for optimal compression using entropy/arithmetic/range coding.

## Why not a 64 bit coder?

While a binary coder is simpler and allows more optimization, it can only encode a binary choice at a time. At best it will produce one bit at a time.

A 64 bit coder is slower to encode a symbol (though this implementation aims to be fast). Yet it can handle a very large alphabet, and if the model permits, encode up to 63 bits at a time.

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

## Learn more

* [Data Compression Explained (Matt Mahoney)](http://mattmahoney.net/dc/dce.html)


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
