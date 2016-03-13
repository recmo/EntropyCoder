name      := Benchmark
desc      := Benchmark based on terriberry's
url       := https://github.com/recmo/EntropyCoder
packages  := EntropyCoder

-include ../Makefile.inc


benchmark: Benchmark ec_tokens.txt
	cat ec_tokens.txt | time ./Benchmark

callgrind: Benchmark ec_tokens.txt
	cat ec_tokens.txt | time valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes ./Benchmark
