// fixlen.cpp

#include <iostream>
#include "Buffer_F.h"

using namespace std;

// class Buffer_F
// public members

void Buffer_F::Clear(){
	IOBuffer::Clear();
}

int Buffer_F::Read(istream &stream)
// write the number of bytes in the buffer field definitions
{
	int recaddr = stream.tellg();
	stream.clear();
	Clear();
	isPackingMode = false;
	stream.read(buffer, usingBufferSize);
	if (!stream.good()) {
		stream.clear();
		return recaddr;
	}
	return recaddr;
}

int Buffer_F::Write(ostream &stream) const
// read the number of bytes in the buffer field definitions
// return the location of the record in the file
{
	int recaddr = stream.tellp();
	stream.write(buffer, usingBufferSize);
	if (!stream.good()) return -1;
	return recaddr;
}

static const char *headerStr = "Fixed";
static const int headerStrSize = 5;

int Buffer_F::ReadHeader(istream &stream)
// read the header and check for consistency
// see WriteHeader for header record structure
{
	char str[headerStrSize+1];
	int result;
	// read the IOBuffer header
	result = IOBuffer::ReadHeader(stream);
	if (result < 0) return -1;
	// read the string "Fixed"
	stream.read (str, headerStrSize);
	if (!stream.good()) return -1;
	if (strncmp(str, headerStr, headerStrSize) != 0) return -1;
	return stream.tellg();
}

// write a buffer header to the beginning of the stream
// A header consists of the 
//	IOBUFFER header	
//	FIXED			5 bytes
int Buffer_F::WriteHeader(ostream &stream) const{
	if (IOBuffer::WriteHeader(stream) != -1) {
		stream.write(headerStr, headerStrSize);
	}
	return stream.tellp();
}

void Buffer_F::Print(ostream &stream) const
{
	IOBuffer::Print(stream);
	stream <<  "--Fixed--";
}

bool Buffer_F::Init(int maxBytes) {
	return IOBuffer::Init(maxBytes);
}

bool Buffer_F::ChangeRecordSize(int recordSize)
{
	if (isInit && maxBytes < recordSize) {
		usingBufferSize = recordSize;
	}
	usingBufferSize = recordSize;
	return true;
}