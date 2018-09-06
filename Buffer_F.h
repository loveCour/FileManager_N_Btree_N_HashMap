// fixlen.h
#ifndef FIXLEN_H
#define FIXLEN_H
#include "iobuffer.h"

using namespace std;
class Buffer_F: public IOBuffer
// Abstract class designed to support fixed length records
{
public:
	Buffer_F():IOBuffer(){}
	Buffer_F(const Buffer_F& buffer):IOBuffer(buffer) {} 
	void Clear(); // clear values from buffer
	int Read(istream &);
	int Write(ostream &) const;
	int ReadHeader(istream &); // read header from stream
	int WriteHeader(std::ostream &) const override; // write a header to the stream
	void Print(ostream &) const;
	int SizeOfBuffer() const; // return size of buffer
 protected:
 	bool Init(int recordSize) override;
	bool ChangeRecordSize(int recordSize);
};

#endif