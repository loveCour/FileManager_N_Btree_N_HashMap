#include <iostream>
#include <fstream>
#include <string>
#include "Buffer_F_Fld.h"
#include "recordng.h"
#include "Buf_File_Manager.h"
#include "BTree.h"

class testClass {
public:
	testClass() {}
	testClass(string a, string b, string c) :fir(a), se(b), th(c) {}
	string fir;
	string se;
	string th;

	void Print() {
		cout << fir << se << th;
	}
	void dirty() {
		fir = "di";
		se = "r";
		th = "ty";
	}
	bool Pack(IOBuffer &Buffer) const{
		int numBytes;
		Buffer.Clear();
		numBytes = Buffer.PackingNextField(&fir, sizeof(fir));
		if (numBytes == -1) return false;
		numBytes = Buffer.PackingNextField(&se, sizeof(se));
		if (numBytes == -1) return false;
		numBytes = Buffer.PackingNextField(&th, sizeof(th));
		if (numBytes == -1) return false;
		return true;
	}
	bool Unpack(IOBuffer &Buffer) const {
		int numBytes;
		numBytes = Buffer.UnpackingNextField((void*)&fir, sizeof(fir));
		if (numBytes == -1) return false;
		numBytes = Buffer.UnpackingNextField((void*)&se, sizeof(se));
		if (numBytes == -1) return false;
		numBytes = Buffer.UnpackingNextField((void*)&th, sizeof(th));
		if (numBytes == -1) return false;
		return true;
	}
	void Print(ostream & str) const {
		str << fir << se << th;
	}
};
using namespace std;

bool BFM_Test();

bool prev_test() {
	Buffer_F_Fld* buffer = new Buffer_F_Fld();
	int hi[3] = { 50, 50, 50 };
	buffer->Init(500, 3, hi);
	testClass* ts = new testClass("first", "Second", "third");
	ts->Print();
	ts->Pack(*buffer);
	fstream File;
	File.open("data.txt", ios::out);
	File.close();
	cout << endl;
	File.open("data.txt", ios::out | ios::in | ios::binary);
	buffer->WriteHeader(File);
	buffer->Write(File);
	File.close();
	File.open("data.txt", ios::out | ios::in | ios::binary);
	Buffer_F_Fld* buffer1 = new Buffer_F_Fld();
	cout << buffer1->ReadHeader(File) << endl;
	buffer1->Read(File);
	testClass *ts1 = new testClass();
	ts1->Unpack(*buffer);
	ts1->Print(cout);
	File.close();
	return true;
}

bool Buf_FileTest() {
	bool result = true;
	Buffer_F_Fld recordbuffer;
	Buffer_F_Fld btreebuffer;
	Recording::InitBuffer(recordbuffer);
	Buf_File_Manager BFM(recordbuffer);
	BTree<string> bt(btreebuffer, 4); //asdsad

	result &= BFM.Create("data.txt", ios::in | ios::out);
	Recording* r = new Recording("LON", "2312", "Romeo and Juliet", "Prokofiev", "Maazel");
	cout << *r << endl;
	r->Pack(recordbuffer);
	int recaddr = BFM.Write();
	result = bt.Insert(r->GetKey(), recaddr); //asdasd
	BFM.Close();
	BFM.Open("data.txt", ios::in | ios::out);
	BFM.Read();
	r->Unpack(recordbuffer);
	cout << *r;
	//내생각에 버퍼파일은 여기에 써주세요 하면 지가 처리해야지 버퍼에 쓰고 
	//그 뒤에 write를 하면 그냥
	//버퍼파일 안쓰고 내가 알아서 해버리는게 낫다고 생각되는데...
	result &= BFM.Close();
	delete r;
	return result;
}

int main() {
	Buf_FileTest();
	return 0;
}


