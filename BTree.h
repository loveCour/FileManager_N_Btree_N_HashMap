#pragma once
// btree.h
#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTNode.h"
#include "recfile.h"
#include "Buffer_F_Fld.h"
#include "IndBuff.h"

const int MaxHeight = 5;
using namespace std;

//새로운 노드를 만들 때 마다 노드를 비트리파일에 추가한다.
//추가할 때 파일에 그 노드를 추가한 위치(주소)를 받아다
//이제 이게 너의 주소라며 그 노드안에 저장한다. 파일의 키도 노드에다 저장한다.
//비트리파일에는 노드가 저장되는거고 노드에 저장된 주소를 보고
//다른 데이터파일에서 데이터를 가져오게 된다.
//데이터 파일은 언제 저장하는거지?
//저장한뒤에 저장한 위치가 나오고 키는 알고 있으니까
//그냥 그걸 비트리에 저장하네 다른 파일에 우선 저장을하고
//저장한 주소랑 키를 따로 비트리에다 저장함
// btree needs to be able to pack, unpack, read and
// 	write index records
// 	also, handle the insert, delete, split, merge,
//	growth of the tree
//	a pool of nodes will be managed
//	keep at least a branch of the tree in memory
template <class keyType>
class BTree
// this is the full version of the BTree
{
public:
	BTree(Buffer_F_Fld &Buffer, int order, int keySize = sizeof(keyType), int unique = 1);
	~BTree();
	int Open(char *name, int mode);
	int Create(const char *name, int mode);
	int Close();
	int Insert(const keyType key, const int recAddr);
	int Remove(const keyType key, const int recAddr = -1);
	int Search(const keyType key, const int recAddr = -1);
	void Print(std::ostream &);
	void Print(std::ostream &, int nodeAddr, int level);
protected:
	typedef BTreeNode<keyType> BTNode;// useful shorthand
	BTNode *FindLeaf(const keyType key);
	// load a branch into memory down to the leaf with key
	BTNode *NewNode();
	BTNode *Fetch(const int recaddr);
	int Store(BTNode *);
	BTNode Root;
	int Height; // height of tree
	int Order; // order of tree
	int PoolSize;
	BTNode **Nodes;	// pool of available nodes
					 // Nodes[1] is level 1, etc. (see FindLeaf)
					 // Nodes[Height-1] is leaf
	RecordFile<BTNode> BTreeFile;
	friend std::ostream &operator<<(std::ostream &, const BTree &);
};

template <class keyType>
BTree<keyType>::BTree(Buffer_F_Fld &Buffer, int order, int keySize, int unique)
	: BTreeFile(Buffer), Root(order)
	//파일에서의 노드 주소, order갯수만큼의 키, order갯수만큼의 주소(레코드파일의)
	//이들 각각이 하나의 필드이다.
{
	Height = 1;
	Order = order;
	PoolSize = MaxHeight * 2;
	Nodes = new BTNode *[PoolSize];
	BTNode::InitBuffer(Buffer, order);//to do btnode 안에서 버퍼를 초기화한다
									  //keySize가 꼭 필요할지 생각해보자
	Nodes[0] = &Root;
}

template <class keyType>
BTree<keyType>::~BTree()
{
	Close();
	delete Nodes;
}

template <class keyType>
int BTree<keyType>::Open(char *name, int mode)
{
	int result;
	result = BTreeFile.Open(name, mode);
	if (!result) return result;
	// load root
	BTreeFile.Read(Root);
	Height = 1; // find height from BTreeFile!
	return 1;
}

template <class keyType>
int BTree<keyType>::Create(const char *name, int mode)
{
	int result;
	result = BTreeFile.Create(name, mode);
	if (!result) return result;
	// append root node
	result = BTreeFile.Write(Root);
	Root.RecAddr = result;
	return result != -1;
}

template <class keyType>
int BTree<keyType>::Close()
{
	int result;
	result = BTreeFile.Rewind();
	if (!result) return result;
	result = BTreeFile.Write(Root);
	if (result == -1) return 0;
	return BTreeFile.Close();
}


template <class keyType>
int BTree<keyType>::Insert(const keyType key, const int recAddr)
{
	int result; int level = Height - 1;
	keyType prevKey;
	BTNode *thisNode, *newNode, *parentNode;
	thisNode = FindLeaf(key);
	newNode = NewNode();

	// test for special case of new largest key in tree
	keyType* largestKey = thisNode->LargestKey();
	bool isNewLargest = (largestKey == nullptr) ? true : false;
	if (isNewLargest) {
		if (!thisNode->isEmpty()) {
			prevKey = thisNode->Keys[thisNode->NumKeys - 1];
		}
	}

	result = thisNode->Insert(key, recAddr);

	// handle special case of new largest key in tree
	if (isNewLargest)
		for (int i = 0; i < Height - 1; i++)
		{
			Nodes[i]->UpdateKey(prevKey, key);
			if (i > 0) Store(Nodes[i]);
		}

	while (result == -1) // if overflow and not root
	{
		// remember the largest key
		largestKey = thisNode->LargestKey();
		// split the node
		// newNode = NewNode();
		thisNode->Split(newNode);
		Store(thisNode); Store(newNode);
		level--; // go up to parent level
		if (level < 0) break;
		// insert newNode into parent of thisNode
		parentNode = Nodes[level];
		result = parentNode->UpdateKey(*largestKey, *(thisNode->LargestKey()));
		result = parentNode->Insert(*(newNode->LargestKey()), newNode->RecAddr);
		thisNode = parentNode;
	}
	Store(thisNode);
	if (level >= 0) return 1;	// insert complete
								// else we just split the root
	int newAddr = BTreeFile.Append(Root); // put previous root into file
										  // insert 2 keys in new root node
	Root.Keys[0] = *(thisNode->LargestKey());
	Root.RecAddrs[0] = newAddr;
	Root.Keys[1] = *(newNode->LargestKey());
	Root.RecAddrs[1] = newNode->RecAddr;
	Root.NumKeys = 2;
	Height++;
	return 1;
}

template <class keyType>
int BTree<keyType>::Remove(const keyType key, const int recAddr)
{
	// left for exercise
	return -1;
}

template <class keyType>
int BTree<keyType>::Search(const keyType key, const int recAddr)
{
	BTNode *leafNode;
	leafNode = FindLeaf(key);
	return leafNode->Search(key, recAddr);
}

template <class keyType>
void BTree<keyType>::Print(ostream &stream)
{
	stream << "BTree of height " << Height << " is " << endl;
	Root.Print(stream);
	if (Height > 1)
		for (int i = 0; i < Root.numKeys(); i++)
		{
			Print(stream, Root.RecAddrs[i], 2);
		}
	stream << "end of BTree" << endl;
}

template <class keyType>
void BTree<keyType>::Print(ostream &stream, int nodeAddr, int level)
{
	BTNode *thisNode = Fetch(nodeAddr);
	stream << "Node at level " << level << " address " << nodeAddr << '	';
	thisNode->Print(stream);
	if (Height > level)
	{
		level++;
		for (int i = 0; i < thisNode->numKeys(); i++)
		{
			Print(stream, thisNode->RecAddrs[i], level);
		}
		stream << "end of level " << level << endl;
	}
}

template <class keyType>
BTreeNode<keyType> *BTree<keyType>::FindLeaf(const keyType key)
// load a branch into memory down to the leaf with key
{
	int recAddr, level;
	for (level = 1; level < Height; level++)
	{
		recAddr = Nodes[level - 1]->Search(key, -1, 0); //inexact search
		Nodes[level] = Fetch(recAddr);
	}
	return Nodes[level - 1];
}

template <class keyType>
BTreeNode<keyType> *BTree<keyType>::NewNode()
{// a fresh node, insert into tree and set RecAddr member
	BTNode *newNode = new BTNode(Order);
	int recAddr = BTreeFile.Append(*newNode);
	newNode->RecAddr = recAddr;
	return newNode;
}

template <class keyType>
BTreeNode<keyType> *BTree<keyType>::Fetch(const int recaddr)
{// load this node from File into a new BTreeNode
	int result;
	BTNode *newNode = new BTNode(Order);
	result = BTreeFile.Read(*newNode, recaddr);
	if (result == -1) return NULL;
	newNode->RecAddr = result;
	return newNode;
}

template <class keyType>
int BTree<keyType>::Store(BTreeNode<keyType> *thisNode)
{
	return BTreeFile.Write(*thisNode, thisNode->RecAddr);
}

template <class keyType>
ostream &operator<<(ostream &os, const BTree<keyType> &bt)
{
	bt.Print(os);
}

#endif