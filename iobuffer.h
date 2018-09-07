// iobuffer.h
#ifndef IOBUFFER_H
#define IOBUFFER_H
#include <iostream>

using namespace std;
class IOBuffer 
// An abstract base class for file buffers
// Record variables can be packed into and extracted from a buffer.
// Input and Output of packed buffers 
// When each field has a value, the buffer can be written into an ostream.
// 
// operations are provided to allow values to be associated with the fields (Pack)
//	and to fetch the values of fields (Unpack)
{ 
public:
	IOBuffer() {
		isInit = false;
	}
	IOBuffer(const IOBuffer& srcBuffer) {
		isInit = false;
		if (buffer != nullptr) {
			delete[] buffer;
		}
		if (srcBuffer.isInit) {
			isInit = srcBuffer.isInit;
			isPackingMode = srcBuffer.isPackingMode;
			maxBytes = srcBuffer.maxBytes;
			buffer = new char[maxBytes];
			memcpy(buffer, srcBuffer.buffer, srcBuffer.maxBytes);
			usingBufferSize = srcBuffer.usingBufferSize;
			nextByte = srcBuffer.nextByte;			
		}		
	}
	~IOBuffer() {
		if(buffer != nullptr) delete[] buffer;
	}
	virtual int PackingNextField(const void *field, int fieldSize = -1) = 0; // Pack 'A next field'
	virtual int UnpackingNextField(void *field, int fieldSize = -1) = 0; // Unpack 'A next field'
	virtual void Print(ostream &) const;
	virtual bool Init(int maxBytes);
	virtual void Clear();
	virtual int Read(istream &) = 0; // Read a record from the stream to buffer
	virtual int Write(ostream &) const = 0; // Write a record to the stream from buffer

	// these are the direct access read and write operations
	virtual int DRead(istream &, int recref); // read specified record
	virtual int DWrite(ostream &, int recref) const; // write specified record

	// these header operations return the number of bytes in the header
	virtual int ReadHeader(istream &); // write a buffer to the stream
	virtual int WriteHeader(ostream &) const; // write a buffer to the stream

 protected:
	char *buffer = nullptr; // character array to hold field values
	int usingBufferSize; // sum of the sizes of packed fields
	int maxBytes; // maximum number of characters in the buffer
	int nextByte; // index of next byte to be packed/unpacked
	bool isInit; // true if buffer is initialized
	bool isPackingMode; // true if in packing mode, false, if unpacking
};

// field packing operations
// pack a field into a buffer
// int packFixed(char *buffer, void *field, int size = -1);
// int packDelimeted(char *buffer, void *field, int size = -1);
// int packLength(char *buffer, void *field, int size = -1);

#endif
