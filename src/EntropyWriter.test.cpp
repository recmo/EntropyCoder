#include "EntropyWriter.h"
#include <UnitTest++.h>
#include <sstream>

SUITE(EntropyWriter) {
using EntropyCoder::Interval;
using EntropyCoder::EntropyWriter;

TEST(Simple)
{
	std::ostringstream out;
	CHECK(out.good());
	{
		EntropyWriter bw(out);
		bw.write(Interval{0, 1000});
	}
	out.flush();
	CHECK(out.good());
}

}
