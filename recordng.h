// recordng.h
#ifndef RECORDING_H
#define RECORDING_H

#include <iostream>
#include <String>
#include "iobuffer.h"
#include "Buffer_F_Fld.h"
// class person with fixed field buffer
using namespace std;
class Recording
	// a recording with a composite key
{
public:
	Recording();
	Recording(const string label_, const string idNum_,
		const string title_, const string composer_, const string artist_);
	string Key() const; // return key of object
	int Unpack(IOBuffer &);
	int Pack(IOBuffer &) const;
	string GetKey() const {
		return idNum + label;
	}
	void Print(std::ostream &) const;
	static int InitBuffer(Buffer_F_Fld &);
private:
	string idNum;
	string title;
	string composer;
	string artist;
	string label;
};

std::ostream &operator<<(std::ostream &, Recording &);
inline std::ostream &operator<<(std::ostream &stream, Recording &rec)
{
	rec.Print(stream); return stream;
}

#endif
