# Stoke

<https://github.com/StanfordPL/stoke-release>

Patch `Interval::update` to produce testcases for `test.inc.cpp`.

At the start add:

```cpp
	std::cout << "base = ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << base << "UL;\n";
	std::cout << "range = ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << range << "UL;\n";
	std::cout << "update_interval(base, range, carry, ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << symbol.base << "UL, ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << symbol.range << "UL);\n";
```

And at the end:

```cpp
	std::cout << "assert(base == ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << base << "UL);\n";
	std::cout << "assert(range == ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << range << "UL);\n";
	std::cout << "assert(carry == ";
	std::cout << (base < t ? "true" : "false" );
	std::cout << ");\n";
	std::cout << "accum += base + range + (carry ? 1 : 0);\n";
}
```

Create `test.inc.cpp`:

```sh
make build
./test  > ./stoke/tests.inc.cpp
./EntropyCoder generate 50 > ./test.txt
cat ./test.txt | ./EntropyCoder encode -p 0.321  >> ./stoke/tests.inc.cpp
cat ./test.txt | ./EntropyCoder encode -p 0.821  >> ./stoke/tests.inc.cpp
cat ./test.txt | ./EntropyCoder encode -p 0.123  >> ./stoke/tests.inc.cpp
```

Run the super-optimizer:

```sh
g++ -std=c++11 -O3 main.cpp
time ./a.out
stoke extract -i ./a.out -o bins
stoke testcase --config testcase.conf
stoke search --config search.conf
```

Test the optimization:

```sh
cp ./a.out ./a.orig
stoke replace --config replace.conf
time ./a.orig
time ./a.out
```
