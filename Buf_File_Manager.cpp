// buffile.cpp
#include "Buf_File_Manager.h"
using namespace std;

Buf_File_Manager::Buf_File_Manager(IOBuffer &from)
// create with a buffer
	: Buffer(from) { }

int Buf_File_Manager::Open(const char *filename, int mode)
// open an existing file and check the header
// a correct header must be on the file
// use ios::nocreate to ensure that a file exists
{
	// these modes are not allowed when opening an existing file
	// if (mode&ios::noreplace||mode&ios::trunc) return false;
	fstream fs(filename, ios::in);
	if (!fs) return false;

	File.open(filename, mode|ios::in);
	if (!File.good()) return false;
	File.seekg(0, ios::beg); File.seekp(0, ios::beg);
	HeaderSize = ReadHeader();
	if (!HeaderSize) // no header and file opened for output
		return false;
	File.seekp(HeaderSize, ios::beg);
	File.seekg(HeaderSize, ios::beg);
	return File.good();
}

int Buf_File_Manager::Create(const char *filename, int mode)
// create a new file and write a header on it.
// use ios::nocreate to ensure that no file exists
{
	if (!(mode & ios::out)) return false; // must include ios::out
	File.open(filename, mode);
	if (!File.good())
	{
		File.close();
		return false;
	}
	HeaderSize = WriteHeader();
	return HeaderSize != 0;
}

int Buf_File_Manager::Close()
{
	File.close();
	return true;
}

int Buf_File_Manager::Rewind()
{
	File.seekg(HeaderSize, ios::beg);
	File.seekp(HeaderSize, ios::beg);
	return 1;
}

// Input and Output operations
int Buf_File_Manager::Read(int recaddr)
// read a record into the buffer
// return the record address
// return <0 if read failed
// if recaddr == -1, read the next record in the File
// if recaddr != -1, read the record at that address
{
	if (recaddr == -1)
		return Buffer.Read(File);
	else
		return Buffer.DRead(File, recaddr);
}
 
int Buf_File_Manager::Write(int recaddr)
 // write the current buffer contents
{
	if (recaddr == -1)
		return Buffer.Write(File);
	else
		return Buffer.DWrite(File, recaddr);
}

int Buf_File_Manager::Append()
// write the current buffer at the end of File
{
	File.seekp(0, ios::end);
	return Buffer.Write(File);
}

// Access to IOBuffer
IOBuffer &Buf_File_Manager::GetBuffer() { return Buffer; }

// protected methods
int Buf_File_Manager::ReadHeader()
{
	return Buffer.ReadHeader(File);	
}

int Buf_File_Manager::WriteHeader()
{
	return Buffer.WriteHeader(File);
}