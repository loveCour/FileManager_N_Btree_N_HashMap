// fixfld.cpp
#include "Buffer_F_Fld.h"
#include <string>
using namespace std;

// private function to calculate the record size from the field sizes
static int SumfieldSizes(int numFields, int *fieldSize)
{
	int sum = 0;
	for (int i = 0; i < numFields; i++)
		sum += fieldSize[i];
	return sum;
}

int Buffer_F_Fld::NumberOfFields() const
// return number of fields 
{
	return numFields;
}

void Buffer_F_Fld::Clear()
// clear fields from buffer
{
	Buffer_F::Clear();
	nextField = 0;
	isPackingMode = true;
}

static const char *headerStr = "Field";
static const int headerStrSize = 5;

//1 parents header
//2 Field
//3 numField
//4 FieldSizes
int Buffer_F_Fld::ReadHeader(istream &stream)
// read the header and check for consistency
// see WriteHeader for header record structure
{
	char *str = new char[headerStrSize+1];
	int _usingBufferSize;
	int _numFields, *_fieldSizes;
	int result;
	// read the Buffer_Fheader
	result = Buffer_F::ReadHeader(stream);
	if (result < 0) return -1;
	// field
	stream.read(str, headerStrSize);
	if (!stream.good()) return -1;
	if (strncmp(str, headerStr, headerStrSize) != 0) return -1;

	//usingBuffeSize
	stream.read((char*)&_usingBufferSize, sizeof(_usingBufferSize));
	if (!stream.good()) return -1;

	// read the record description
	stream.read((char*)&_numFields, sizeof(_numFields));
	if (!stream) return -1; // failed to read numFields

	_fieldSizes = new int[_numFields];
	for (int i = 0; i < _numFields; i ++)
	{
		stream.read((char*)&_fieldSizes[i], sizeof(_fieldSizes[i]));
		if (!stream.good()) return -1;
	}

	if (isInit) // check header for consistency
	{
		if (numFields != _numFields) return -1;
		for (int j = 0; j < numFields; j ++)
			if (fieldSizes[j] != _fieldSizes[j]) return -1;
		return stream.tellg(); // everything matches
	}
	// else initialize the buffer from the header
	int _maxBytes = SumfieldSizes(_numFields, _fieldSizes);
	if (!Init(_maxBytes, _numFields, _fieldSizes)) return -1;
	return stream.tellg();
}

int Buffer_F_Fld::WriteHeader(ostream &stream) const
//1 parents header
//		1.1 iobuffer
//		1.2 fixed 
//2 Field
//3 recordSize
//4 numField
//5 FieldSizes
{
	int result;
	if (!isInit) return -1; // cannot write unitialized buffer
	// write the parent (Buffer_F) header
	result = Buffer_F::WriteHeader(stream);
	if (!result) return -1;
	stream.write(headerStr, headerStrSize);
	if (!stream.good()) return -1;
	cout << "usingBufferSize " << usingBufferSize;
	stream.write((char *)&usingBufferSize, sizeof(usingBufferSize));
	if (!stream.good()) return -1;
	cout << "packing numfields " << numFields;
	stream.write((char*)&numFields, sizeof(numFields));
	for (int i = 0; i < numFields; i++)
	{
		cout << "packing fieldsize " << fieldSizes[i] << " ";
		stream.write((char*)&fieldSizes[i], sizeof(fieldSizes[i]));
	}
	if (!stream) return -1;
	return stream.tellp();
}

int Buffer_F_Fld::PackingNextField(const void *field, int fieldSize)
// set the value of the next field of the buffer;
//    if size != -1, it must be the same as the packSize
// return number of bytes packed, -1 if error
{
	// cout << "Pack numFields " << numFields << " field " << (char *)field << endl;
	if ((nextField == numFields) || !isPackingMode || !isInit) // buffer is full or not packing mode
		return -1;
	int packSize = fieldSizes[nextField]; // number bytes to be packed
	if (fieldSize > packSize) return -1;
	if (fieldSize == -1) fieldSize = packSize;
	memcpy(&buffer[nextByte], field, fieldSize); // move bytes to buffer
	nextByte += fieldSize;
	int remainSize = packSize - fieldSize;
	if (remainSize > 0) {
		char *tmp = new char[remainSize];
		for (int i = 0; i < remainSize; i++) {
			tmp[i] = '0';
		}
		memcpy(&buffer[nextByte], tmp, remainSize);
		delete tmp;
		nextByte += remainSize;
	}	
	nextField++;
	if (nextField == numFields) // all fields packed
	{
		isPackingMode = false;
		nextField = nextByte = 0;
	}
	return packSize;
}

int Buffer_F_Fld::UnpackingNextField(void *field, int fieldSize)
// extract the value of the next field of the buffer
// return the number of bytes extracted, -1 if error
{
	if (!isInit) return -1;
	isPackingMode = false;
	if (nextField == numFields) // buffer is full 
		return -1;
	int start = nextByte; // first byte to be unpacked
	int packSize = fieldSizes[nextField]; // number bytes to be unpacked
	if (fieldSize > packSize) return -1;
	memcpy(field, &buffer[start], fieldSize);
	nextByte += packSize;
	nextField++;
	if (nextField == numFields) Clear(); // all fields unpacked
	return packSize;
}

void Buffer_F_Fld::Print(ostream &stream) const
{
	Buffer_F::Print(stream);
	stream << endl;
	stream << "\tmax fields "<< maxFields << " and actual " << numFields << endl;
	for (int i = 0; i < numFields; i++)
		stream << "\tfield " << i << " size " << fieldSizes[i] << endl;
	buffer[usingBufferSize] = 0;
	stream << "NextByte " << nextByte << endl;
	stream << "Buffer '" << buffer << "'" << endl;
}

bool Buffer_F_Fld::Init(int maxBytes, int _MaxFields, int *_fieldSizes) 
// construct with fields of specific size
{
	Buffer_F::Init(maxBytes);
	// initialize
	if (!_fieldSizes) return false;
	Clear();
	if (_MaxFields < 0) maxFields = 0;
	maxFields = _MaxFields;

	if (this->fieldSizes != nullptr) delete[] this->fieldSizes;
	this->fieldSizes = new int[maxFields];
	// add fields
	for (int j = 0; j < maxFields; j++)
		AddField(_fieldSizes[j]);
	isInit = true;
	return true;
}

int Buffer_F_Fld::AddField(int fieldSize)
{
	if (numFields == maxFields) return false;
	if (usingBufferSize + fieldSize > maxBytes) return false;
	fieldSizes[numFields] = fieldSize;
	numFields++;
	usingBufferSize += fieldSize;
	return true;
}