from ctypes import cdll
ec = cdll.LoadLibrary("libEntropyCoder.so")

class EntropyWriter:
	def __init__(self, filename):
		self.handle = ec.entropy_writer_create(filename.encode('ascii'))
		assert(self.handle != 0)
	
	def __enter__(self):
		return self
	
	def __exit__(self, exc_type, exc_val, exc_tb):
		r = ec.entropy_writer_finalize_destroy(self.handle)
		assert(r == 0)
	
	def write(self, lowest, highest):
		assert(lowest >= 0)
		assert(lowest + 3 <= highest)
		assert(highest <= 2**64)
		r = ec.entropy_writer_write(self.handle, lowest, highest)
		assert(r == 0)

class EntropyReader:
	def __init__(self, filename):
		self.handle = ec.entropy_reader_create(filename.encode('ascii'))
		assert(self.handle != 0)
	
	def __enter__(self):
		return self
	
	def __exit__(self, exc_type, exc_val, exc_tb):
		r = ec.entropy_reader_destroy(self.handle)
		assert(r == 0)
	
	def eof(self):
		return ec.entropy_reader_eof(self.handle) == 1
	
	def value(self):
		return ec.entropy_reader_value(self.handle)
	
	def next(self, lowest, highest):
		assert(lowest >= 0)
		assert(lowest + 3 <= highest)
		assert(highest <= 2**64)
		r = ec.entropy_reader_next(self.handle, lowest, highest)
		assert(r == 0)
