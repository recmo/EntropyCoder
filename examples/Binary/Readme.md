# Binary

A simple utility to generate, encode and decode independent static random bits.

## Build

	make

## The normal way

Generate 1000 biased random bits

	./Binary generate 1000 -p 0.42 > ./in.txt

Compress them

	cat in.txt | ./Binary encode -p 0.42 > ./bits.ec

View compressed bits

	hexdump -C bits.ec

Decompress them

	cat bits.ec | ./Binary decode -p 0.42 > ./out.txt

Test

	diff in.txt out.txt


## The wrong way around

Since the whole thing is bijective, and any file is a valid input for
decompression, we can do the following:

Generate a random file

	dd if=/dev/urandom of=./random.in bs=1k count=1

Decompress the file as if it contained biased bits:

	cat random.in | ./Binary decode -p 0.42 > ./random.txt

Recompress the output

	cat random.txt | ./Binary encode -p 0.42 > ./random.out

Test

	diff random.in random.out
