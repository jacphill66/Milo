#include "DRT.h"
#include "B.h"
#include "BTreeNode.h"

//Calculates m based on key and data sizes
int BTree::calculate_m(int key_size, int data_size){
	return floor(4086/(key_size + data_size + 4));
}

//BTree constructor
BTree::BTree(fstream* rf, int key_size, int data_size, bool new_tree){
	this->root = NULL;
	this->rfstream = rf;
	if(new_tree){
		this->m = calculate_m(key_size, data_size);
		this->key_size = key_size;
		this->data_size = data_size;
		this->top_address = 4096;	
		this->init_root();
		this->initialize_top_block();
	}
	else{
		this->read_top_block();
	}
	this->mi = floor((double)m/2);
}

int BTree::next_address(){
	if(top_empty_slot != 0){
		int t = top_empty_slot;
		top_empty_slot = read_block(top_empty_slot)->parent;
		return t;
	}
	top_address += 4096;
	return top_address;
}

//Initializes an empty root
void BTree::init_root(){
	int_or_char parent;
	parent.i = 0;
    short_or_char size;
	size.s = 0;	
	int_or_char children;
	children.i = 0;
	
	char empty_root[4096];
	
	empty_root[0] = parent.c[0];
	empty_root[1] = parent.c[1];
	empty_root[2] = parent.c[2];
	empty_root[3] = parent.c[3];
	
	empty_root[4] = size.c[0];
	empty_root[5] = size.c[1];
	
	empty_root[6] = children.c[0];
	empty_root[7] = children.c[1];
	empty_root[8] = children.c[2];
	empty_root[9] = children.c[3];

	this->root_address = 4096;
    this->rfstream->seekp(this->root_address);
	this->rfstream->write(empty_root, 4096);
}

//Deletes the tree
BTree::~BTree(){
	delete this->root;
}

//Searches the tree for a key, returns a DRT
DRT* BTree::search(string key){
	this->root = this->read_block(this->root_address);
	DRT* result = this->root->search(this, this->root, key);
	delete root;
	return result;
}

//Modify for adding, deleting and modifying existing keys, that is, not adding or deleting
DRT* BTree::modify(string k, string v){
	DRT* s = search(k);
	if(s->getdata() == "" && v != ""){
		this->root = this->read_block(this->root_address);
		this->root->add(this, this->root, this->root_address, k, v);
		if(this->root->size > this->m){	
			BTreeNode* new_root = new BTreeNode(0, this->m);
			int m = ceil((double)this->root->size/2)-1;
			new_root->keys[0] = this->root->keys[m];
			new_root->data[0] = this->root->data[m];
			new_root->children = new int[this->m+1];
			new_root->size = 1;
			new_root->children[0] = this->root_address;
			this->root_address = next_address();
			new_root->children[1] = next_address();			
			BTreeNode* c1 = this->root->left_half(this);
			c1->parent = this->root_address;
			BTreeNode* c2 = this->root->right_half(this);            
			c2->parent = this->root_address;
			this->root = new_root;
			this->write_block(this->root, this->root_address);
			this->write_block(c1, this->root->children[0]);
			this->write_block(c2, this->root->children[1]);		
			delete c1;
			delete c2;
			delete new_root;
		}
	}
	else if(s->getdata() != "" && v == ""){
		root = read_block(root_address);
		if(this->root->size == 1 && this->root->children == NULL){
				string v = this->root->data[0];
				delete this->root;
				this->init_root();
		}
		else{
			this->root = this->read_block(this->root_address);
			this->root->delete_(this, this->root, this->root_address, k);
			if(this->root->size == 0 && this->root->children[1] == 0){
				int target = this->root->children[0];
				BTreeNode* c1 = this->read_block(this->root->children[0]);
				if(c1->children != NULL){
					for(int i = 0; i < c1->size + 1; i++){
						BTreeNode* q = this->read_block(c1->children[i]);
						q->parent = this->root_address;
						write_block(q, c1->children[i]);
						delete q;
					}
				}
				this->root = c1;
				this->root->parent = 0;
				this->write_block(root, this->root_address);
				delete c1;
				BTreeNode* e = new BTreeNode(this->top_empty_slot, this->m);
				this->write_block(e, target);
				delete e;			
			}
		}
	}
	else if(s->getdata() != "" && v != ""){
		root->modify(this, root, root_address, k, v);
	}
	initialize_top_block();
	return s;
}

void BTree::initialize_top_block(){
	//store: m, key_size, data_size, top_address of the empty nodes, root ptr
	//m: int, key_size:int, data_size:int, top_address:int, root_ptr:int
	char buff[4096];
	int_or_char m;
	m.i = this->m;
	buff[0] = m.c[0];
	buff[1] = m.c[1];
	buff[2] = m.c[2];
	buff[3] = m.c[3];

	int_or_char k_size;
	k_size.i = this->key_size;
	buff[4] = k_size.c[0];
	buff[5] = k_size.c[1];
	buff[6] = k_size.c[2];
	buff[7] = k_size.c[3];

	int_or_char d_size;
	d_size.i = this->data_size;
	buff[8] = d_size.c[0];
	buff[9] = d_size.c[1];
	buff[10] = d_size.c[2];
	buff[11] = d_size.c[3];
	
	int_or_char t_addr;
	t_addr.i = this->top_address;
	buff[12] = t_addr.c[0];
	buff[13] = t_addr.c[1];
	buff[14] = t_addr.c[2];
	buff[15] = t_addr.c[3];
	
	int_or_char r_addr;
	r_addr.i = this->root_address;
	buff[16] = r_addr.c[0];
	buff[17] = r_addr.c[1];
	buff[18] = r_addr.c[2];
	buff[19] = r_addr.c[3];
	
	this->rfstream->seekp(0);
	this->rfstream->write(buff, 4096);
}


void BTree::read_top_block(){
	char block[4096];
	this->rfstream->seekg(0);
	this->rfstream->read(block, 4096);
	
	int_or_char m;
	m.c[0] = block[0];
	m.c[1] = block[1];
	m.c[2] = block[2];
	m.c[3] = block[3];

	this->m = m.i;

	
	int_or_char k_size;
	k_size.c[0] = block[4];
	k_size.c[1] = block[5];
	k_size.c[2] = block[6];
	k_size.c[3] = block[7];

	this->key_size = k_size.i;

	int_or_char d_size;
	d_size.c[0] = block[8];
	d_size.c[1] = block[9];
	d_size.c[2] = block[10];
	d_size.c[3] = block[11];
	
	this->data_size = d_size.i;
	
	int_or_char t_addr;
	t_addr.c[0] = block[12];
	t_addr.c[1] = block[13];
	t_addr.c[2] = block[14];
	t_addr.c[3] = block[15];
	
	this->top_address = t_addr.i;
	
	int_or_char r_addr;
	r_addr.c[0] = block[16];
	r_addr.c[1] = block[17];
	r_addr.c[2] = block[18];
	r_addr.c[3] = block[19];
	
	this->root_address = r_addr.i;

}

//Writes a block
void BTree::write_block(BTreeNode* node, int address){
	char block [4096];
	int_or_char parent_address;
	short_or_char size;
	parent_address.i = node->parent;
	size.s = (short)node->size;
	
	block[0] = parent_address.c[0];
	block[1] = parent_address.c[1];
	block[2] = parent_address.c[2];
	block[3] = parent_address.c[3];
	
	block[4] = size.c[0];
	block[5] = size.c[1];
	
	int b_index = 6;

	for(int i = 0; i < size.s; i++){
		for(int j = 0; j < key_size; j++){
			block[b_index] = node->keys[i][j];
			b_index++;
		}
	}

	for(int i = 0; i < size.s; i++){
		for(int j = 0; j < data_size; j++){
			block[b_index] = node->data[i][j];
			b_index++;
		}
	}
	
	if(node->children != NULL){
		for(int i = 0; i < size.s+1; i++){
			int_or_char child_address;
			child_address.i = node->children[i];
			block[b_index] = child_address.c[0];
			block[b_index+1] = child_address.c[1];
			block[b_index+2] = child_address.c[2];
			block[b_index+3] = child_address.c[3];
			b_index+=4;
		}
	}
	else{
		int_or_char zero;
		zero.i = 0;
		block[b_index] = zero.c[0];
		block[b_index+1] = zero.c[1];
		block[b_index+2] = zero.c[2];
		block[b_index+3] = zero.c[3];
		b_index+=4;
	}
	this->rfstream->seekp(address);
	this->rfstream->write(block, 4096);
}

//Reads a block
BTreeNode* BTree::read_block(int address){
	//Block = [4 bytes of parent address + 2 bytes of size + M bytes of keys + M bytes of data + M+1 * 4 bytes of child_addresses]
	BTreeNode* node = new BTreeNode(0, this->m+1);
	char block[4096];
	this->rfstream->seekg(address);
	this->rfstream->read(block, 4096);

	int_or_char parent_address;
	short_or_char size;
	
	parent_address.c[0] = block[0];
	parent_address.c[1] = block[1];
	parent_address.c[2] = block[2];
	parent_address.c[3] = block[3];
	
	size.c[0] = block[4];
	size.c[1] = block[5];
	
	node->parent = parent_address.i;
	node->size = (int)size.s;
	
	int b_index = 6;

	for(int i = 0; i < node->size; i++){
		for(int j = 0; j < key_size; j++){
			node->keys[i] += block[b_index];		
			b_index++;
		}
	}
	
	for(int i = 0; i < node->size; i++){
		for(int j = 0; j < data_size; j++){
			node->data[i] += block[b_index];
			b_index++;
		}
	}
	int_or_char child_address;
	child_address.c[0] = block[b_index];
	child_address.c[1] = block[b_index+1];
	child_address.c[2] = block[b_index+2];
	child_address.c[3] = block[b_index+3];

	
	if(child_address.i == 0){
		node->children = NULL;
		return node;
	}
	node->children = new int[node->m+1];
	for(int i = 0; i < node->size+1; i++){
		int_or_char child_address;
		child_address.c[0] = block[b_index];
		child_address.c[1] = block[b_index+1];
		child_address.c[2] = block[b_index+2];
		child_address.c[3] = block[b_index+3];
		node->children[i] = child_address.i;	
		b_index+=4;

	}

	return node;
}

//Debugging Method, prints the keys and addresses of the tree
void BTree::print_tree(){
	this->root->print_node(this, this->root_address);
}

//Debugging Method, pretty-prints the tree
string BTree::to_string(){
	return this->root->to_string(this, this->root_address);
}

//debugging method
void BTree::read_in_all_nodes(){
	int i = this->top_address;
	while(i >= 4096){
		BTreeNode* b = this->read_block(i);
		cout << "Parent Address: ";
		cout << b->parent << endl;
		delete b;
		i -= 4096;
	}
}