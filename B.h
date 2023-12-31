#ifndef _B_
#define _B_

#include "DRT.h"
#include <iostream>
#include <cstdlib>
#include <cstring> 
#include <string> 
#include <fstream>
#include <io.h>
#include <cmath>
#include <vector>

using namespace std;

class BTreeNode;

union int_or_char{
	int i;
	char c[4];
};

union short_or_char{
	short s;
	char c[2];
};

class BTree{
 public:
	int m;
	int mi;
	BTreeNode* root;
	int top_address;
	fstream* rfstream;
	int root_address;
	int key_size;
	int data_size;
	int top_empty_slot = 0;
	
	//BTree Initialization and Destructuring
	int calculate_m(int key_size, int data_size);
	void init_root();
	BTree(fstream* rf, int key_size, int data_size, bool new_tree);
	~BTree();
	
	void initialize_top_block();
	void read_top_block();

	//Returns
	int next_address();

	//Printing and debugging
	string to_string();
	void print_tree();
	
	//Read and Write Blocks
	BTreeNode* read_block(int address);
	void write_block(BTreeNode* node, int address);	
	
	//Modify the DB(adding, deleting, modifying)
	DRT* modify(string key, string value);
	
	//Search the DB
	DRT* search(string key);
	
	//void add(string key, string value);	
	//void delete_(string key);
	void read_in_all_nodes();
};



#endif