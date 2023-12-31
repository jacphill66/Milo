#include "DRT.h"
#include "BTreeNode.h"

//Constructs a BTree Node of size m
BTreeNode::BTreeNode(int parent, int m){
	this->parent = parent;
	this->m = m;
	this->keys = new string[m];
	this->data = new string[m];
	this->mi = floor((double)this->m/2);
	this->children = NULL;
    this->size = 0;
}

//Destructor
BTreeNode::~BTreeNode(){
	free(this->keys);
	free(this->data);
	free(this->children);
}

//Shifts the children of a node to the right
void BTreeNode::right_shift(int index){
	int* new_children = new int[this->m+1];
    int i = 0;
    int j = 0;
    int k = 0;
    while( j < this->size + 1){
		if(k == index){
            new_children[j] = 0;
			j += 1;
		}
        else{
            new_children[j] = this->children[i];
            i += 1;
            j += 1;
		}
		k += 1;
	}
	free(this->children);
	this->children = new_children;
}

//Returns a new node with the left half of the keys, data and children-> For splitting->
BTreeNode* BTreeNode::left_half(BTree* tree){
	BTreeNode* new_node = new BTreeNode(0, tree->m);
    int* new_children = NULL;
    if(this->children != NULL){
		new_children =  new int[tree->m+1];
	}
    int i = 0;
    int end = ceil((double)this->size/2)-1;
    while (i < end){
		new_node->keys[i] = this->keys[i];
        new_node->data[i] = this->data[i];
        if(this->children != NULL){
			new_children[i] = this->children[i];
		}
        i += 1; 
	}
    if(this->children != NULL){
		new_children[i] = this->children[i];
	}
		
    new_node->size = i;
	new_node->children = new_children;
    return new_node;
	
}

//Returns a new node with the right half of the keys, data and children->
BTreeNode* BTreeNode::right_half(BTree* tree){
	BTreeNode* new_node = new BTreeNode(0, tree->m);
    int* new_children = NULL;
    if(this->children != NULL){
		new_children = new int[tree->m + 1];
    }
	int i = 0;
    int j = ceil((double)this->size/2);
    while(j < this->size){
		new_node->keys[i] = this->keys[j];
        new_node->data[i] = this->data[j];
        if (this->children != NULL){
			new_children[i] = this->children[j];
		}
        i += 1;
        j += 1;
	}

    if (this->children != NULL){
		new_children[i] = this->children[j];
	}
    new_node->children = new_children;
    new_node->size = i;
    return new_node;
}

//Inserts a key and data into a node
void BTreeNode::insert(string key, string value){
	int index = this->find_place(key);
	string* new_keys = new string[this->m];
	string* new_data = new string[this->m];
    int i = 0;
    int j = 0;
    int k = 0;
    while(j < this->size + 1){
		if(k == index){
			new_keys[j] = key;
            new_data[j] = value;
            j += 1;
		}
        else{
            new_keys[j] = this->keys[i];
            new_data[j] = this->data[i];
            i += 1;
            j += 1;
		}	
        k += 1;
	}
	this->size += 1;
	free(this->keys);
	free(this->data);
    this->keys = new_keys;
    this->data = new_data;
}

//Splits a child node
void BTreeNode::split_child(BTree* tree, BTreeNode* c, int index, BTreeNode* node, int address, string key){
	int m = ceil((double)c->size/2)-1;
    node->insert(c->keys[m], c->data[m]);
    BTreeNode* lhs = c->left_half(tree);
    BTreeNode* rhs = c->right_half(tree);
	node->right_shift(index);
	lhs->parent = address;
	rhs->parent = address;	
    node->children[index] = tree->next_address();
    tree->write_block(lhs, node->children[index]);
    tree->write_block(rhs, node->children[index+1]);
	if(lhs->children!=0){
		for(int j = 0; j < lhs->size+1; j++){
			BTreeNode* t = tree->read_block(lhs->children[j]);
			t->parent = node->children[index];
			tree->write_block(t, lhs->children[j]);
			free(t);
		}
	}
	if(rhs->children!=0){
		for(int j = 0; j < rhs->size+1; j++){
			BTreeNode* t = tree->read_block(rhs->children[j]);
			t->parent = node->children[index+1];
			tree->write_block(t, rhs->children[j]);
			free(t);
		}
	}
    if (node->size <= tree->m){
		tree->write_block(node, address);
	}
	delete rhs;
	delete lhs;
}

//Adds a key, value pair to a node
void BTreeNode::add(BTree* tree, BTreeNode* node, int address, string key, string value){
	if(node->children == NULL){
        node->insert(key, value);	
		if(node->size <= tree->m){
			tree->write_block(node, address);
		}	
	}
    else{
		int index = node->find_place(key);
		BTreeNode* c = tree->read_block(node->children[index]);
        c->add(tree, c, node->children[index], key, value);
		if(c->size > tree->m){
			node->split_child(tree, c, index, node, address, key);
		}
		delete c;
	}
}

//Grow_child helper method
int BTreeNode::index_helper(int index){
	if(index == 0){
		return 0;
	}
    else{
		return index-1;
	}
}

//Checks to see if a key is in a node
bool BTreeNode::in_keys(string key, int index){
	return this->size > index && this->keys[index] == key;
}

//Removes a key, data from a nodes keys and data
string BTreeNode::remove(string key){
	int index = this->find_place(key);
    string* new_keys = new string[this->m];
	string* new_data = new string[this->m];
	string d;
    int i = 0;
    int j = 0;
    int k = 0;
    while (i < this->size){
		if (k == index){
			d = this->data[k];
			i +=1;
		}
        else{
			new_keys[j] = this->keys[i];
            new_data[j] = this->data[i];
            i += 1;
            j += 1;
		}
        k += 1;
	}
	this->size -= 1;
	free(this->keys);
	free(this->data);
    this->keys = new_keys;
    this->data = new_data;
	return d;
}

//Finds a keys place in a node
int BTreeNode::find_place(string key){
	int i = 0;
	while(i < this->size && this->keys[i] < key){
		i += 1;
	}
	return i;
}

void BTreeNode::deallocate_node(BTree* t, int address){
	BTreeNode* n = new BTreeNode(t->top_empty_slot, 0);
	t->top_empty_slot = address;
	t->write_block(n, address);
	delete n;
}


//Merges two nodes togethor, quite the mess but it works (I think)
BTreeNode* BTreeNode::merge(BTree* tree, BTreeNode* c1, int index1, BTreeNode* c2, int index2, BTreeNode* parent, int address, bool j_bool){
	BTreeNode* n = new BTreeNode(0, tree->m);
	string* new_keys = new string[c1->size + c2->size + 1];
	string* new_data = new string[c1->size + c2->size + 1];
	free(n->keys);
	free(n->data);
	n->keys = new_keys;
	n->data = new_data;
    n->children = NULL;
	n->size = c1->size + c2->size;
	if (c1->size + c2->size < parent->m){
		free(new_keys);
		free(new_data);
		n->keys = new string[parent->m];
		n->data = new string[parent->m];
	}
    if (c1->children != NULL){
		if(c1->children != NULL){
			n->children = new int[c1->size + c2->size + 1];
		}
		if((c1->size + c2->size + 1) < parent->m+1){
			n->children = new int[parent->m+1];
		}
	}
	int i = 0;
    int k = 0;
    while(i < c1->size){
		n->keys[i] = c1->keys[i];
        n->data[i] = c1->data[i];
		if(c1->children != NULL){
			n->children[k] = c1->children[i];
			k += 1;
		}
		i += 1;
	}
    if(c1->children != NULL && (!j_bool)){
		n->children[k] = c1->children[i];
        k += 1;
	}
    int j = 0;
    while(j < c2->size){
		n->keys[i] = c2->keys[j];
        n->data[i] = c2->data[j];
        if (c2->children != NULL){
			n->children[k] = c2->children[j];
			k += 1;
		}
        i += 1;
        j += 1;
	}
    if(c2->children != NULL && j_bool){
		n->children[k] = c2->children[j];
        k += 1;
	}
	n->parent = address;
	parent->children[index2] = 0;
    int z = index2;
    while(z < parent->size+1){
        if(parent->children[z] == 0 && parent->children[z+1] != 0){
			parent->children[z] = parent->children[z + 1];
            parent->children[z+1] = 0;
		}	
		z += 1;
	}
	return n;
}

BTreeNode* BTreeNode::merge_helper(BTree* tree, BTreeNode* c, int index, BTreeNode* n, int address){
	if(index == 0){
		BTreeNode* c2 = tree->read_block(n->children[index+1]);
		BTreeNode* x = n->merge(tree, c, index, c2, index+1, n, address, true);
		delete c2;
		return x;
	}
    else{
		BTreeNode* c2 = tree->read_block(n->children[index-1]);
		BTreeNode* x = n->merge(tree, c2, index-1, c, index, n, address, false);
		delete c2;
		return x;
	}
}

//This method grows a child node that is one too small. It merges said child node with an adjacent child node, then it splits if nessecary
void BTreeNode::grow_child(BTree* tree, BTreeNode* c, int index, BTreeNode* node, int address, string key){
	int i = node->index_helper(index);
    string k = node->keys[i];
    string v = node->data[i];
    node->remove(k);
    c->insert(k, v);	
	int target;
	if(index==0){
		target = node->children[1];
	}
	else{
		target = node->children[index];
	}
	c = node->merge_helper(tree, c, index, node, address);
    if(index == 0){
		if(c->size > tree->m){
			node->split_child(tree, c, index, node, address, key);
		}
		else{
			tree->write_block(node, address);
			tree->write_block(c, node->children[0]);	
			if(c->children != NULL){
				for(int f = 0; f < c->size+1; f++){
					BTreeNode* t = tree->read_block(c->children[f]);
					t->parent = node->children[0];
					tree->write_block(t, c->children[f]);
					free(t);
				}
			}
		}
		deallocate_node(tree, target);
	}
    else{
		if(c->size > tree->m){
			node->split_child(tree, c, index-1, node, address, key);
		}
		else{
			tree->write_block(node, address);
			tree->write_block(c, node->children[index-1]);
			if(c->children != NULL){
				for(int f = 0; f < c->size+1; f++){
					BTreeNode* t = tree->read_block(c->children[f]);
					t->parent = node->children[index-1];
					tree->write_block(t, c->children[f]);
					free(t);
				}
			}
		}	
		deallocate_node(tree, target);
	}
}

//Deletes a key, value from a node and adjusts sorounding nodes accordingly
void BTreeNode::delete_(BTree* tree, BTreeNode* node, int address, string key){
	int index = node->find_place(key);  
	int i = 0;
	if(node->in_keys(key, index)){
		if(node->children == 0){
			string d = node->remove(key);
			if(node->size >= tree->mi || (node->parent == 0 && node->children==0)){
				tree->write_block(node, address);
			}
		}
		else{			
			BTreeNode* c = tree->read_block(node->children[index+1]);
			c->delete_inner_key(tree, c, index+1, node->children[index+1], node, address, key);
			if(c->size < tree->mi){
				node->grow_child(tree, c, index+1, node, address, key);
			}
		}
	}
	else{
		BTreeNode* c = tree->read_block(node->children[index]);	
		c->delete_(tree, c, node->children[index], key);
        if(c->size < tree->mi){
			node->grow_child(tree, c, index, node, address, key);
		}
	}
}

//Helper method that deletes an inner key and adjusts sorounding nodes accordingly
void BTreeNode::delete_inner_key(BTree* tree, BTreeNode* c, int index, int child_address, BTreeNode* inner_node, int address, string key){
	if(c->children == NULL){		
		string k = c->keys[0];
        string v = c->data[0];
        c->remove(k);
        string d = inner_node->remove(key);
        inner_node->insert(k, v);
		tree->write_block(inner_node, address);
		if(c->size >= tree->mi){
			tree->write_block(c, child_address);
		}
	}
    else{
		BTreeNode* c_c = tree->read_block(c->children[0]);
		c_c->delete_inner_key(tree, c_c, 0, c->children[0], inner_node, address, key);
		if (c_c->size < tree->mi){
			c_c->grow_child(tree, c_c, 0, c, child_address, key);
		}
		delete c_c;
	}
}


/*
	DRT Stuff
*/


//Goes left all the way down and returns the node
BTreeNode* BTreeNode::latwd(BTree* t, BTreeNode* n){
	if(n->children != 0){
		return latwd(t, t->read_block(n->children[0]));
	}
    else{
		return n;
	}
}
//Goes right all the way down and returns the node
BTreeNode* BTreeNode::ratwd(BTree* t, BTreeNode* n){
	if(n->children != 0){
		return ratwd(t, t->read_block(n->children[(n->size-1)+1]));
	}
	else{
		return n;
	}
}

//Goes left once then right all the way down and returns the node
BTreeNode* BTreeNode::loratwd(BTree* t, BTreeNode* n, string key){
	int index = n->find_place(key);    
    if(n->children != 0){
		return ratwd(t, t->read_block(n->children[index]));
	}
    else{
		return n;
	}
}

//Goes right once then left all the way down and returns the node
BTreeNode* BTreeNode::rolatwd(BTree* t, BTreeNode* n, string key){
	int index = n->find_place(key);
    if( n->children != 0){
		return latwd(t, t->read_block(n->children[index+1]));
	}
    else{
		return n;
	}
}

//Finds the next key of a key contained in a leaf node
string BTreeNode::find_next_of_leaf(BTree* t, BTreeNode* n, string key){
	if(key >= n->keys[n->size-1] && n->parent != 0){
		return this->find_next_of_leaf(t, t->read_block(n->parent), key);
	}
    else if(n->parent == 0 && key > n->keys[n->size-1]){
		return "";
	}
    else{

		int i = 0;
		while( i < n->size){
			if(n->keys[i] > key){
				return n->keys[i];
			}
			i+=1;
	    }
		return "";
	}
}

//Finds the previous key of a key contained in a leaf node
string BTreeNode::find_previous_of_leaf(BTree* t, BTreeNode* n, string key){
	if(key <= n->keys[0] && n->parent != 0){
		return this->find_previous_of_leaf(t, t->read_block(n->parent), key);
	}
    else if(n->parent == 0 && key < n->keys[0]){
		return "";
	}
    else{
		int i = n->size-1;
        while(i >= 0){
            if(n->keys[i] < key){
				return n->keys[i];
			}
			i -= 1;
		}
		return "";
	}
}

//Finds the node that contains the key
BTreeNode* BTreeNode::find_node(BTree* t, BTreeNode* n, string key){
	int index = n->find_place(key); 
    if(n->in_keys(key, index)){
		return n;
	}
    else{
		return this->find_node(t, t->read_block(n->children[index]), key);
	}
}

//Finds the previous key of a key
string BTreeNode::find_previous(BTree* t, BTreeNode* n, string key){
	if(n->parent == 0 && n->size == 0){
		return "";
	}
    else if(n->children != 0){
		BTreeNode* x = loratwd(t, n, key);
		string str = x->keys[x->size-1];
		delete x;
		return str;
	}
    else{
		return find_previous_of_leaf(t, n, key);
	}
}

//Finds the next key of a key
string BTreeNode::find_next(BTree* t, BTreeNode* n, string key){
	if(n->parent == 0 && n->size == 0){
		return "";
	}
    else if(n->children != 0){
		return rolatwd(t, n, key)->keys[0];
	}
    else{
		return find_next_of_leaf(t, n, key);
	}
}

string BTreeNode::first(BTree* t, BTreeNode* n, string key){
	DRT* s = search(t, n, key);
	while(s->getprev() != ""){
		key = s->getprev();
		s = t->search(key);
	}
	delete s;
	return key;
}

string BTreeNode::last(BTree* t, BTreeNode* n, string key){
	DRT* s = search(t, n, key);
	while(s->getnext() != ""){
		key = s->getnext();
		s = t->search(key);
	}
	delete s;
	return key;
}

DRT* BTreeNode::augment_drt(BTree* t, BTreeNode* n, string key){
	int index = n->find_place(key);    
	if(n->size == 0){
		return new DRT("", "", "");
	}
	else{
		if(index == 0){
			return new DRT("", n->keys[0], find_previous(t, n, n->keys[0]));
		}
		else if(index > n->size-1){
			return new DRT("", find_next(t, n, n->keys[n->size-1]), n->keys[n->size-1]);
		}
		else{
			return new DRT("", n->keys[index], n->keys[index-1]);
		}
	}
}

//Searches the tree for a key and returns a DRT
DRT* BTreeNode::search(BTree* t, BTreeNode* n, string key){
	if(key == ""){
		if(n->size > 0){
			return new DRT("", first(t, n, n->keys[0]), last(t, n, n->keys[0]));
		}
		else{
			return new DRT("", "", "");
		}
	}
	int index = n->find_place(key);    
	if(!n->in_keys(key, index) && n->children==0){
		return augment_drt(t, n, key);
	}
    else if(n->in_keys(key, index)){
		return new DRT(n->data[index], find_next(t, n, key), find_previous(t, n, key));
	}
    else{
		return search(t, t->read_block(n->children[index]), key);
	}
}

void BTreeNode::modify(BTree* t, BTreeNode* n, int address, string key, string value){
	//just don't call this if search says the node doesn't exist
	int index = n->find_place(key);    
	if(n->in_keys(key, index)){    
		n->data[index] = value;
		t->write_block(n, address);
	}
    else{
		modify(t, t->read_block(n->children[index]), n->children[index], key, value);
	}
}

/*
	Debugging Methods
*/


//Prints a node and its children (with addresses)
void BTreeNode::print_node(BTree* tree, int address){
	string s = "";
	BTreeNode* n = tree->read_block(address);
	//cout << "Parent Address: " << n->parent << " Address: " << address << "( ";
	cout << "Address: " << address << "( ";
	for(int i = 0; i < n->size; i++){
		cout << n->keys[i][0] << " ";
	}
	cout << " )" << endl;
	if(n->children != NULL){
		for(int i = 0; i < n->size+1; i++){
			this->print_node(tree, n->children[i]);
		}
	}
	cout << endl;
	delete n;
}

//Prints the keys of a node
void BTreeNode::print_keys(BTreeNode* n){
	cout << "Keys: ";
	for(int i = 0; i < n->size; i++){
		cout << n->keys[i][0] << " ";
	}
	cout << endl;
}

//Pretty prints a node (and its children...)
string BTreeNode::to_string(BTree* tree, int address){
	string s = "";
	BTreeNode* n = tree->read_block(address);
	if(n->children != NULL){
		s += ("( " + n->to_string(tree, n->children[0]));
        for(int i = 0; i < n->size; i++){
			s += " ";
			s += n->keys[i][0];
			s += " ";
			s += n->to_string(tree, n->children[i+1]);
			s += " ";
		}
        s += ")";
		delete n;
        return s;
	}
    else{
		s += "( ";
        for(int i = 0; i < n->size; i++){
            s += n->keys[i][0];
			s += " ";
		}
		s += ")";
		delete n;
		return s;
	}
}
