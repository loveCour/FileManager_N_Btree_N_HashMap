// iobuffer.cpp
#include "iobuffer.h"
using namespace std;

void IOBuffer::Print(ostream &stream) const
{
	stream << "MaxBytes " << maxBytes << " usingBufferSize " << usingBufferSize;
}

bool IOBuffer::Init(int size)
{
	Clear();
	if (buffer != nullptr) {
		delete[] buffer;
	}
	maxBytes = size;
	if (maxBytes < 0) maxBytes = 0;
	buffer = new char[maxBytes];
	usingBufferSize = 0;
	isInit = true;
	return true;
}

void IOBuffer::Clear() {
	
	nextByte = 0;
	isPackingMode = true;
}

int IOBuffer::DRead(istream &stream, int recref)
// read specified record
{
	stream.seekg(recref, ios::beg);
	if ((int)stream.tellg() != recref) return -1;
	return Read(stream);
}

int IOBuffer::DWrite(ostream &stream, int recref) const
// write specified record
{
	stream.seekp(recref, ios::beg);
	if ((int)stream.tellp() != recref) return -1;
	return Write(stream);
}

static const char *headerStr = "IOBuffer";
static const int headerSize = 8;

int IOBuffer::ReadHeader(istream &stream)
{
	bool isReadingSuccess;
	int readHeaderSize;
	char str[headerSize + 1];
	stream.seekg(0, ios::beg);
	stream.read(str, headerSize);
	bool isSuccess = stream.good();
	if(isSuccess){
		isReadingSuccess = strncmp(str, headerStr, headerSize) == 0;
	}
	if (isReadingSuccess) readHeaderSize = headerSize;
	else readHeaderSize = -1;
	return readHeaderSize;
}

int IOBuffer::WriteHeader(ostream &stream) const
{
	int writtenHeaderSize;
	stream.seekp(0, ios::beg);
	stream.write(headerStr, headerSize);
	return stream.tellp();
}