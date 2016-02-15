#include "wrapper.h"
#include "EntropyWriter.h"
#include "EntropyReader.h"
#include <stddef.h>
#include <stdexcept>
#include <fstream>
#include <memory>
using EntropyCoder::EntropyWriter;
using EntropyCoder::EntropyReader;

/******************************************************************************/

struct HEntropyWriter {
	std::unique_ptr<std::ostream> out;
	std::unique_ptr<EntropyWriter> ew;
};

HEntropyWriter* entropy_writer_create(const char* filename)
{
	try {
		std::unique_ptr<std::ostream> out{new std::ofstream{filename}};
		std::unique_ptr<EntropyWriter> ew{new EntropyWriter{*out.get()}};
		return new HEntropyWriter{std::move(out), std::move(ew)};
	}
	catch(...) {
		return NULL;
	}
}

int entropy_writer_finalize_destroy(HEntropyWriter* handle)
{
	try {
		handle->ew->finalize();
		handle->ew.reset();
		handle->out.reset();
		delete handle;
	}
	catch(...) {
		return -1;
	}
	return 0;
}

int entropy_writer_write(HEntropyWriter* handle, uint64_t start, uint64_t end)
{
	try {
		handle->ew->write(start, end);
	}
	catch(...) {
		return -1;
	}
	return 0;
}

/******************************************************************************/

struct HEntropyReader {
	std::unique_ptr<std::istream> in;
	std::unique_ptr<EntropyReader> er;
};

HEntropyReader* entropy_reader_create(const char* filename)
{
	try {
		std::unique_ptr<std::istream> in{new std::ifstream{filename}};
		std::unique_ptr<EntropyReader> ew{new EntropyReader{*in.get()}};
		return new HEntropyReader{std::move(in), std::move(ew)};
	}
	catch(...) {
		return NULL;
	}
}

int entropy_reader_destroy(HEntropyReader* handle)
{
	try {
		handle->er.reset();
		handle->in.reset();
		delete handle;
	}
	catch(...) {
		return -1;
	}
	return 0;
}

int entropy_reader_eof(const HEntropyReader* handle)
{
	try {
		return handle->er->eof() ? 1 : 0;
	}
	catch(...) {
		return 1;
	}
}

uint64_t entropy_reader_value(const HEntropyReader* handle)
{
	try {
		return handle->er->value();
	}
	catch(...) {
		return -1;
	}
}

int entropy_reader_next(HEntropyReader* handle, uint64_t start, uint64_t end)
{
	try {
		handle->er->next(start, end);
	}
	catch(...) {
		return -1;
	}
	return 0;
}
