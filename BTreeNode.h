#ifndef _BTREENODE_
#define _BTREENODE_

#include "B.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>

using namespace std;


class BTreeNode{
 public:
	int m;
	int mi;
	int size;
	int parent;
	int* children;
	string* keys;
	string* data;
	
	
	BTreeNode(int parent, int m);
	~BTreeNode();
	
	//For printing and debuging
	string to_string(BTree* tree, int address);
	void print_node(BTree* tree, int address);
	void print_keys(BTreeNode* n);
	
	//General use methods
	int find_place(string key);
	bool in_keys(string key, int index);

	//Searching
	DRT* search(BTree* t, BTreeNode* n, string key);
	
	//Modifying (existing keys, that is, not adding or deleting)
	void modify(BTree* t, BTreeNode* n, int address, string key, string value);

	//Adding
	void right_shift(int index);
	BTreeNode* left_half(BTree* tree);
	BTreeNode* right_half(BTree* tree);
	void split_child(BTree* tree, BTreeNode* c, int index, BTreeNode* n, int address, string key);
	void insert(string key, string value);
	void add(BTree* tree, BTreeNode* node, int address, string key, string value);
	
	
	//Deleting
	//remove can be void
	string remove(string key);
	BTreeNode* merge(BTree* tree, BTreeNode* c1, int index1, BTreeNode* c2, int index2, BTreeNode* n, int address, bool j_bool);
	BTreeNode* merge_helper(BTree* tree, BTreeNode* c, int index, BTreeNode* node, int address);
	int index_helper(int index);
	void grow_child(BTree* tree, BTreeNode* c, int index, BTreeNode* node, int address, string key);
	void delete_(BTree* tree, BTreeNode* node, int address, string key);
	void delete_inner_key(BTree* tree, BTreeNode* c, int index, int child_address, BTreeNode* inner_node, int address, string key);
	void deallocate_node(BTree* tree, int address);

	
	//For calculating the next and previous and other DRT stuff
	DRT* augment_drt(BTree* t, BTreeNode* n, string key);
	BTreeNode* latwd(BTree* tree, BTreeNode* n);
	BTreeNode* ratwd(BTree* tree, BTreeNode* n);
	BTreeNode* loratwd(BTree* tree, BTreeNode* n, string key);
	BTreeNode* rolatwd(BTree* tree, BTreeNode* n, string key);
	string find_next_of_leaf(BTree* tree, BTreeNode* n, string key);
	string find_previous_of_leaf(BTree* tree, BTreeNode* n, string key);
	BTreeNode* find_node(BTree* tree, BTreeNode* n, string key);
	string find_previous(BTree* tree, BTreeNode* n, string key);
	string find_next(BTree* tree, BTreeNode* n, string key);
	string first(BTree* t, BTreeNode* n, string key);
	string last(BTree* t, BTreeNode* n, string key);
};

#endif