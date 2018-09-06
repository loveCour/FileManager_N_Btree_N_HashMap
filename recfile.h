#ifndef RECFILE_H
#define RECFILE_H

#include "Buf_File_Manager.h"
#include "iobuffer.h"

// template class to support direct read and write of records
// The template parameter RecType must support the following
//	int Pack (IOBuffer &); pack record into buffer
//	int Unpack (IOBuffer &); unpack record from buffer

template <class RecType>
class RecordFile: public Buf_File_Manager
{
public:
	RecordFile(IOBuffer &buffer) : Buf_File_Manager(buffer) { }
	int Read(RecType &record, int recaddr = -1);
	int Write(const RecType &record, int recaddr = -1);
	int Append(const RecType &record);
	
};

// template method bodies
template <class RecType>
int RecordFile<RecType>::Read(RecType &record, int recaddr)
{
	int writeAddr, result;
	writeAddr = Buf_File_Manager::Read(recaddr);
	if (!writeAddr) return -1;
	result = record.Unpack(Buffer);
	if (!result) return -1;
	return writeAddr;
}

template <class RecType>
int RecordFile<RecType>::Write(const RecType &record, int recaddr)
{
	int result;
	result = record.Pack(Buffer);
	if (!result) return -1;
	return Buf_File_Manager::Write(recaddr);
}

template <class RecType>
int RecordFile<RecType>::Append(const RecType &record)
{
	int result;
	result = record.Pack(Buffer);
	if (!result) return -1;
	return Buf_File_Manager::Append();
}

#endif