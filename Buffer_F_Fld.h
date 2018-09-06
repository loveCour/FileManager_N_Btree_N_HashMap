// fixfld.h
#ifndef FIXFLD_H
#define FIXFLD_H

#include <stdlib.h>
#include <iostream>
#include "Buffer_F.h"

class Buffer_F_Fld: public Buffer_F{
public:
	Buffer_F_Fld() :Buffer_F() {}
	Buffer_F_Fld(const Buffer_F_Fld & buffer) :Buffer_F(buffer) {
		memcpy(fieldSizes, buffer.fieldSizes, maxFields*sizeof(int));
		int *fieldSizes; // array to hold field sizes
		int maxFields; // maximum number of fields
		int nextField; // index of next field to be packed/unpacked
	}
	~Buffer_F_Fld() {
		if (fieldSizes != nullptr) delete[] fieldSizes;
	}
	void Clear(); // clear values from buffer	
	int ReadHeader(std::istream &); // write a buffer to the stream
	int WriteHeader(std::ostream &) const; // write a buffer to the stream
	int PackingNextField(const void *field, int fieldSize) override; // set the value of the next field of the buffer;
	int UnpackingNextField(void *field, int fieldSize) override; // extract the value of the next field of the buffer
	void Print(std::ostream &) const;
	int NumberOfFields() const; // return number of defined fields
	bool Init(int maxBytes, int numFields, int *fieldSize);
 protected:
	int AddField(int fieldSize); // define the next field

	int *fieldSizes = nullptr; // array to hold field sizes
	int maxFields; // maximum number of fields
	int numFields; // actual number of defined fields
	int nextField; // index of next field to be packed/unpacked
};

#endif