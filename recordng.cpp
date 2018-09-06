//recording.cpp
#include "recordng.h"
#include <strstream>
#include <string.h>
using namespace std;

Recording::Recording() :
	label(""), idNum(""), title(""), composer(""), artist("")
{}

Recording::Recording(const string label_, const string idNum_,
	const string title_, const string composer_, const string artist_)
	: label(label_), idNum(idNum_), title(title_),
	composer(composer_), artist(artist_)
{}

string Recording::Key() const
{// produce key as concatenation of Label and IdNum
	ostrstream key;
	key << label << idNum << ends;
	return key.str();
}

int Recording::Pack(IOBuffer &Buffer) const
{// return true if all succeed, false o/w
	int numBytes;
	Buffer.Clear();

	const char* tmp = idNum.c_str();
	int a = idNum.size();
	numBytes = Buffer.PackingNextField(tmp, idNum.size() + 1);
	if (numBytes == -1) return false;

	tmp = title.c_str();
	numBytes = Buffer.PackingNextField(tmp, title.size() + 1);
	if (numBytes == -1) return false;

	tmp = composer.c_str();
	numBytes = Buffer.PackingNextField(tmp, composer.size() + 1);
	if (numBytes == -1) return false;

	tmp = artist.c_str();
	numBytes = Buffer.PackingNextField(tmp, artist.size() + 1);
	if (numBytes == -1) return false;

	tmp = label.c_str();
	numBytes = Buffer.PackingNextField(label.c_str(), label.size() + 1);
	if (numBytes == -1) return false;

	return true;
}

int Recording::Unpack(IOBuffer & Buffer)
{// unpack with maximum size, and add null termination to strings
	int numBytes;
	char tmp[30] = "";
	numBytes = Buffer.UnpackingNextField(tmp, 7);
	if (numBytes == -1) return false;
	idNum = tmp;

	numBytes = Buffer.UnpackingNextField(tmp, 30);
	if (numBytes == -1) return false;
	title = tmp;

	numBytes = Buffer.UnpackingNextField(tmp, 30);
	if (numBytes == -1) return false;
	composer = tmp;

	numBytes = Buffer.UnpackingNextField(tmp, 30);
	if (numBytes == -1) return false;
	artist = tmp;

	numBytes = Buffer.UnpackingNextField(tmp, 7);
	if (numBytes == -1) return false;
	label = tmp;

	return true;
}

void Recording::Print(ostream & stream) const
{
	stream << label << '|' << idNum << '|' << title << '|'
		<< composer << '|' << artist;
}

int Recording::InitBuffer(Buffer_F_Fld &buffer) {
	int sizes[5] = { 7,30,30,30,7 };
	return buffer.Init(105, 5, sizes);
}