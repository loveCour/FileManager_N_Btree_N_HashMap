#pragma once
// BTNode.h
#ifndef BTNODE_H
#define BTNODE_H

template <class keyType>
class BTree;

#include <iostream>
#include "SimpInd.h"
#include "recfile.h"
#include "Buffer_F_Fld.h"

template <class keyType>
class BTreeNode : public SimpleIndex <keyType>
// this is the in-memory version of the BTreeNode
{
public:
	BTreeNode(int maxKeys, int unique = 1);
	~BTreeNode();
	// Insert and Remove return
	//	0 for failure
	//	-1 for overflow
	//	1 for success
	int Insert(const keyType key, int recAddr);
	int Remove(const keyType key, int recAddr = -1);
	//int Search (const keyType key) const;
	void Print(std::ostream &) const;
	int Split(BTreeNode<keyType> *newNode); // move keys into newNode
	int Merge(BTreeNode<keyType> *fromNode); // move keys from fromNode
	int UpdateKey(keyType oldKey, keyType newKey, int recAddr = -1);
	int Pack(IOBuffer &buffer) const;
	int Unpack(IOBuffer &buffer);
	keyType* LargestKey() {
		keyType* result;
		if (isEmpty()) {
			result = nullptr;
		}
		else {
			int largestKeyInd = this->NumKeys - 1;
			result = &(this->Keys[largestKeyInd]);
		}
		return result;
	}
	bool isEmpty() {
		bool result = true;
		if (this->NumKeys == 0) {
			result = true;
		}
		else {
			result = false;
		}
		return result;
	}
	static int InitBuffer(Buffer_F_Fld &buffer, int maxKeys);
protected:
	int NextNode;	// address of next node at same level
	int RecAddr;	// address of this node in the BTree file
	int MinKeys;	// minimum number of keys in a node
	int MaxBKeys;	// maximum number of keys in a node
	int Init();
	void Clear() { NumKeys = 0; RecAddr = -1; }
	friend class BTree<keyType>;
};

template <class keyType>
BTreeNode<keyType>::BTreeNode(int maxKeys, int unique)
	: SimpleIndex<keyType>(maxKeys + 1, unique)
{
	Init();
}

template <class keyType>
BTreeNode<keyType>::~BTreeNode() { }

template <class keyType>
int BTreeNode<keyType>::Insert(const keyType key, int recAddr)
{
	int result;
	result = SimpleIndex<keyType>::Insert(key, recAddr);
	if (!result) return 0; // insert failed
	if (this->NumKeys >= this->MaxKeys) return -1; // node overflow
	return 1;
}

template <class keyType>
int BTreeNode<keyType>::Remove(const keyType key, int recAddr)
{
	int result;
	result = SimpleIndex<keyType>::Remove(key, recAddr);
	if (!result) return 0; // remove failed
	if (this->NumKeys < MinKeys) return -1; // node underflow
	return 1;
}

template <class keyType>
void BTreeNode<keyType>::Print(ostream &stream) const
{
	SimpleIndex<keyType>::Print(cout);
}

template <class keyType>
int BTreeNode<keyType>::Split(BTreeNode<keyType> *newNode)
{
	// check for sufficient number of keys
	if (this->NumKeys < this->MaxKeys) return 0;
	// find the first Key to be moved into the new node
	int midpt = (this->NumKeys + 1) / 2;
	int numNewKeys = this->NumKeys - midpt;
	// check that number of keys for newNode is ok
	if (numNewKeys > newNode->MaxBKeys || numNewKeys < newNode->MinKeys)
		return 0;
	// move the keys and recaddrs from this to newNode
	for (int i = midpt; i < this->NumKeys; i++)
	{
		newNode->SimpleIndex<keyType>::Keys[i - midpt] = this->Keys[i];
		newNode->SimpleIndex<keyType>::RecAddrs[i - midpt] = this->RecAddrs[i];
	}
	// set number of keys in the two Nodes
	newNode->NumKeys = numNewKeys;
	this->NumKeys = midpt;
	// Link the nodes together
	return 1;
}

template <class keyType>
int BTreeNode<keyType>::Merge(BTreeNode<keyType> *fromNode)
{
	// check for too many keys
	if (NumKeys + fromNode->NumKeys > MaxKeys - 1) return 0;
	// move keys and recaddrs from fromNode to this
	for (int i = 0; i < fromNode->NumKeys; i++)
	{
		Keys[NumKeys + i] = fromNode->Keys[i];
		RecAddrs[NumKeys + i] = fromNode->RecAddrs[i];
	}
	// adjust number of keys
	NumKeys += fromNode->NumKeys;
	// adjust links
	NextNode = fromNode->NextNode;
	return 1;
}

template <class keyType>
int BTreeNode<keyType>::UpdateKey(keyType oldKey, keyType newKey, int recAddr)
{
	// look for the old key
	int recaddr = this->Search(oldKey, recAddr);
	if (recaddr < 0) return 0; // key and recaddr not found
	Remove(oldKey, recAddr);
	Insert(newKey, recaddr);
	return 1;
}

template <class keyType>
int BTreeNode<keyType>::Init()
{
	NextNode = -1;
	RecAddr = -1;
	MaxBKeys = this->MaxKeys - 1;
	MinKeys = MaxBKeys / 2;
	return 1;
}

template <class keyType>
BTreeNode<keyType> *CreateBTreeNode(int maxKeys, int unique)
{
	return new BTreeNode<keyType>(maxKeys, unique);
}

template <class keyType>
int BTreeNode<keyType>::Pack(IOBuffer &buffer) const
{//to do pack 할 때 고정크기 버퍼에 맞게 고쳐야한다.
	int result;
	buffer.Clear();
	result = buffer.PackingNextField(&this->NumKeys);
	for (int i = 0; i < this->NumKeys; i++)
	{// note only pack the actual keys and recaddrs
		result = result && buffer.PackingNextField(&this->Keys[i]);
		result = result && buffer.PackingNextField(&this->RecAddrs[i]);
	}
	return result;
}

template <class keyType>
int BTreeNode<keyType>::Unpack(IOBuffer &buffer)
{
	int result;
	result = buffer.UnpackingNextField(&this->NumKeys);
	for (int i = 0; i < this->NumKeys; i++)
	{// note only pack the actual keys and recaddrs
		result = result && buffer.UnpackingNextField(&this->Keys[i]);
		result = result && buffer.UnpackingNextField(&this->RecAddrs[i]);
	}
	return result;
}

template <class keyType>
int BTreeNode<keyType>::InitBuffer(Buffer_F_Fld &buffer, const int maxKeys)
{// initialize a buffer for the btree node
	const int numOfField = maxKeys * 2 + 1;
	int *sizes = new int[numOfField];
	sizes[0] = sizeof(int);//이 노드의 현재 레코드의 갯수
	for (int i = 1; i < numOfField; i += 2){
		sizes[i] = sizeof(keyType);//레코드의 키
		sizes[i+1] = sizeof(int);// 이 키의 레코드의 파일에 저장된 주소
	}
	int bufferSize = maxKeys * (sizeof(keyType) + sizeof(int)) + sizeof(int);
	int result = buffer.Init(bufferSize, numOfField, sizes);
	delete[] sizes;
	return result;
}


#endif