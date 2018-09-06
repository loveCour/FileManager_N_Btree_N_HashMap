// indbuff.cpp
#ifndef INDBUFF_CPP
#define INDBUFF_CPP

#include "IndBuff.h"
using namespace std;

template <class keyType>
IndexBuffer<keyType>::IndexBuffer(int maxKeys, int keySize)
	: Buffer_F_Fld(1 + 2 * maxKeys,
		sizeof(int) + maxKeys * keySize + maxKeys * sizeof(int))
	// buffer fields consist of 
	//    numKeys, actual number of keys
	//    Keys [maxKeys] key fields size = maxKeys * keySize
	//    RecAddrs [maxKeys] record address fields size = maxKeys*sizeof(int)
{
	MaxKeys = maxKeys;
	//cout<<"MaxKeys "<<MaxKeys<<endl;
	KeySize = keySize;
	AddField(sizeof(int));
	for (int i = 0; i < maxKeys; i++)
	{
		AddField(KeySize);
		AddField(sizeof(int));
	}
}

template <class keyType>
int IndexBuffer<keyType>::Pack(const SimpleIndex<keyType> &index)
{
	int result;
	Clear();
	result = Buffer_F_Fld::Pack(&index.NumKeys);
	for (int i = 0; i < index.NumKeys; i++)
	{// note only pack the actual keys and recaddrs
		result = result && Buffer_F_Fld::Pack(&index.Keys[i]);
		result = result && Buffer_F_Fld::Pack(&index.RecAddrs[i]);
	}
	return result;
}

template <class keyType>
int IndexBuffer<keyType>::Unpack(SimpleIndex<keyType> &index)
{
	int result;
	result = Buffer_F_Fld::Unpack(&index.NumKeys);
	//cout <<"unpack numkeys "<<index.NumKeys<<endl;
	for (int i = 0; i < index.NumKeys; i++)
	{// note only pack the actual keys and recaddrs
	 //cout <<"unpack keys "<<i<<" key address "<<&(index.Keys[i])<<endl;
		result = result && Buffer_F_Fld::Unpack(&index.Keys[i]);
		result = result && Buffer_F_Fld::Unpack(&index.RecAddrs[i]);
		//cout <<"unpack keys "<<i<<" key "<<index.Keys[i]<<endl;
	}
	return result;
}

template <class keyType>
void IndexBuffer<keyType>::Print(ostream &stream) const
{
	stream << "IndexBuffer: KeySize " << KeySize
		<< " MaxKeys " << MaxKeys << endl;
	Buffer_F_Fld::Print(stream);
}

#endif