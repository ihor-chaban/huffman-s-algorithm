/////////////////////////////////////////////////
//											   //
//	Implementation of data compression and	   //
//	decompression using Huffman's algorithm	   //
//											   //
//			Author - Ihor Chaban			   //
//											   //
/////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>

// max range of binary tree array size in bytes
#define MAX_RANGE_OF_TREE unsigned short int

using namespace std;

struct Node{
	unsigned long int number;
	char data;
	Node *left, *right;
};

struct MyCompare{
	bool operator () (Node* left, Node* right) const{
		return (left->number < right->number);
	}
};

void printTree(Node* root, unsigned int indent = 0){
	if (root){
		if (root->right){
			printTree(root->right, indent + 3);
		}

		for (unsigned int i = 0; i < indent; i++){
			cout << " ";
		}

		cout << root->number;
		if (root->left == NULL && root->left == NULL){
			cout << "(" << root->data << ")";
		}
		cout << endl;

		if (root->left){
			printTree(root->left, indent + 3);
		}
	}
}

char* getExtension(char* input){
	int i = 0, j = 0, start_index = 0;
	for (i = 0; input[i] != '\0'; i++){
		if (input[i] == '.'){
			start_index = i + 1;
		}
	}

	if (start_index > 1){
		char* result = new char[i - start_index];
		for (i = start_index; input[i] != '\0'; i++){
			result[j++] = input[i];
		}
		result[j] = '\0';
		return result;
	}
	else
		return "";
}

char* removeExtension(char* input){
	int i = 0, j = 0, start_index = 0;
	for (i = 0; input[i] != '\0'; i++){
		if (input[i] == '.'){
			start_index = i;
		}
	}

	if (!start_index){
		return input;
	}

	char* result = new char[i - (i - start_index)];
	for (i = 0; i < start_index; i++){
		result[j++] = input[i];
	}
	result[j] = '\0';

	return result;
}

char* changeExtension(char* input, char* extension){
	char* result;
	int size1 = 0, size2 = 0;
	for (int i = 0; removeExtension(input)[i] != '\0'; i++){
		size1++;
	}
	for (int i = 0; extension[i] != '\0'; i++){
		size2++;
	}
	result = new char[size1 + size2 + 1];

	int j = 0;
	for (int i = 0; i < size1; i++){
		result[j++] = input[i];
	}
	result[j++] = '.';
	for (int i = 0; i < size2; i++){
		result[j++] = extension[i];
	}
	result[j] = '\0';
	return result;
}

vector <bool> treeToBin(Node* root){
	vector <Node> first;
	first.push_back(*root);
	int start_index = 0;

	for (int i = start_index; i != first.size(); i++){
		if (first[i].left){
			first.push_back(*first[i].left);
		}
		if (first[i].right){
			first.push_back(*first[i].right);
		}
		if ((first[i].left) || (first[i].right)){
			start_index++;
		}
	}

	vector <bool> second;
	for (unsigned int i = 0; i < first.size(); i++){
		if (first[i].left == NULL && first[i].right == NULL){
			second.push_back(1);
			
			for (int j = 7; j >= 0; j--){
				second.push_back((first[i].data >> j) & 1);
			}
		}
		else {
			second.push_back(0);
		}
	}

	return second;
}

Node* buildTree(char* filename){
	map <char, vector <bool> > table;
	map <char, unsigned long int> count;
	char temp;

	fstream input(filename, ios_base::in | ios_base::binary);
	if (input.is_open()){
		while (input.read(&temp, sizeof(char))){
				count[temp]++;
		}
		count[EOF]++;
	}
	else {
		return NULL;
	}
	input.close();

	list <Node*> nodeList;
	for (map<char, unsigned long int>::iterator itr = count.begin(); itr != count.end(); itr++){
		Node *temp = new Node();
		temp->number = itr->second;
		temp->data = itr->first;
		nodeList.push_back(temp);
	}

	while (nodeList.size() != 1){
		nodeList.sort(MyCompare());
		Node* temp = new Node();
		temp->left = nodeList.front();
		temp->number = nodeList.front()->number;
		nodeList.pop_front();
		temp->right = nodeList.front();
		temp->number += nodeList.front()->number;
		nodeList.pop_front();
		nodeList.push_front(temp);
	}

	return nodeList.front();
}

void buildTable(Node* root, map <char, vector <bool> > &table){
	static vector <bool> code;
	if (root){
		if (root->left){
			code.push_back(0);
			buildTable(root->left, table);
		}

		if (root->right){
			code.push_back(1);
			buildTable(root->right, table);
		}

		if (root->left == NULL && root->right == NULL){
			table[root->data] = code;
		}

		if (!code.empty()){
			code.pop_back();
		}
	}
}

Node* restoreTree(char* input, MAX_RANGE_OF_TREE size){
	list <bool> tree_bin;
	for (unsigned int i = 0; i < size; i++){
		for (unsigned int j = 0; j < 8; j++){
			tree_bin.push_back(input[i] & (1 << (7 - j)));
		}
	}

	Node* root = NULL;
	list <Node*> z1, z2;
	if (!tree_bin.empty() && !tree_bin.front()){
		Node* temp = new Node();

		Node* tempL = new Node();
		Node* tempR = new Node();

		temp->left = tempL;
		temp->right = tempR;

		z1.push_back(tempL);
		z1.push_back(tempR);

		tree_bin.pop_front();
		root = temp;
	}

	while (!z1.empty()){
		while (!z1.empty()){
			if (!tree_bin.front()){
				Node* tempL = new Node();
				Node* tempR = new Node();

				z1.front()->left = tempL;
				z1.front()->right = tempR;

				z2.push_back(tempL);
				z2.push_back(tempR);

				z1.pop_front();
				tree_bin.pop_front();
			}
			else {
				z1.front()->data = 0;
				tree_bin.pop_front();
				for (int i = 0; i < 8; i++){
					z1.front()->data |= tree_bin.front() << (7 - i);
					tree_bin.pop_front();
				}
				z1.pop_front();
			}
		}
		swap(z1, z2);
	}

	return root;
}

void compress(char* filename){
	// opening source file
	fstream input(filename, ios_base::in | ios_base::binary);
	if (!input.is_open()){
		cout << "File " << filename << " does not exist!" << endl;
		return;
	}

	// getting source file extension
	char* ext = getExtension(filename);
	char ext_length = strlen(ext);
	char* out_filename = changeExtension(filename, "bin");

	// building binary tree from input file
	Node* tree = buildTree(filename);

	// building map of chars and its binary code
	map <char, vector <bool> > table;
	buildTable(tree, table);

	// converting binary tree into binary array
	vector <bool> tree_bin = treeToBin(tree);

	// converting size of binary tree array from bits to bytes
	MAX_RANGE_OF_TREE tree_size = static_cast <MAX_RANGE_OF_TREE> (ceil(tree_bin.size() / 8.0));

	// opening binary output
	fstream output(out_filename);
	output.open(out_filename, ios_base::out | ios_base::binary | ios_base::trunc);
	/*if (!output.is_open()){
	output.open(out_filename, ios_base::out | ios_base::binary);
	}
	else {
	cout << "File " << out_filename << " already exists!" << endl;
	return;
	}*/

	// writing source file extension and its length
	output.write((char*)&ext_length, sizeof(char));
	for (int i = 0; i < ext_length; i++){
		output.write((char*)&ext[i], sizeof(char));
	}

	// writing binary tree array and its size
	output.write((char*)&tree_size, sizeof(MAX_RANGE_OF_TREE));
	char buffer = 0, c_buf = 0;
	for (unsigned int i = 0; i < tree_bin.size(); i++){
		buffer |= tree_bin[i] << (7 - c_buf++);
		if (c_buf == 8){
			output.write((char*)&buffer, sizeof(char));
			c_buf = 0;
			buffer = 0;
		}
	}
	if (c_buf){
		output.write((char*)&buffer, sizeof(char));
	}

	// replacement of source data by its binary codes
	buffer = 0; c_buf = 0;
	char temp;
	while (input.read(&temp, sizeof(char))){
		for (vector <bool>::iterator i = table[temp].begin(); i != table[temp].end(); i++){
			buffer |= (*i) << (7 - c_buf++);
			if (c_buf == 8){
				output.write((char*)&buffer, sizeof(char));
				c_buf = 0;
				buffer = 0;
			}
		}
	}

	// adding EOF marker to encoded data
	temp = EOF;
	for (vector <bool>::iterator i = table[temp].begin(); i != table[temp].end(); i++){
		buffer |= (*i) << (7 - c_buf++);
		if (c_buf == 8){
			output.write((char*)&buffer, sizeof(char));
			c_buf = 0;
			buffer = 0;
		}
	}

	// writing last incomplete byte if exists
	if (c_buf){
		for ( ; c_buf < 8; c_buf++){
			buffer |= (!table[EOF][0]) << (7 - c_buf++);
		}
		output.write((char*)&buffer, sizeof(char));
	}

	input.close();
	output.close();
}

void decompress(char* filename){
	// opening source file
	fstream input(filename, ios_base::in | ios_base::binary);
	if (!input.is_open()){
		cout << "File " << filename << " does not exist!" << endl;
		return;
	}

	// getting source extension and setting output filename
	char ext_length;
	char* ext;
	char* out_filename;
	input.read((char*)&ext_length, sizeof(char));
	if (ext_length){
		ext = new char[ext_length + 1];
		input.read(ext, sizeof(char)* ext_length);
		ext[ext_length] = '\0';
		out_filename = changeExtension(filename, ext);
	}
	else {
		out_filename = removeExtension(filename);
	}

	// reading size of binary tree array
	MAX_RANGE_OF_TREE tree_size;
	input.read((char*)&tree_size, sizeof(MAX_RANGE_OF_TREE));

	// restoring binary tree from array
	char* temp = new char[tree_size];
	input.read(temp, sizeof(char)* tree_size);
	Node* tree = restoreTree(temp, tree_size);
	delete[]temp;

	// checking if source file already exists
	fstream output(out_filename);
	output.open(out_filename, ios_base::out | ios_base::binary | ios_base::trunc);
	/*if (!output.is_open()){
	output.open(out_filename, ios_base::out | ios_base::binary);
	}
	else {
	cout << "File " << out_filename << " already exists!" << endl;
	return;
	}*/

	Node* p = tree;
	int count = 0;
	char byte;
	bool bit = false, end = false;

	list <char> buffer;
	while (input.read(&byte, sizeof(char))){
		for (int i = 0; i < 8; i++){
			bit = byte & (1 << (7 - i));

			if (bit == 0){
				p = p->left;
			}

			if (bit == 1){
				p = p->right;
			}

			if (p->left == NULL && p->right == NULL){
				if (p->data != EOF){
					buffer.push_back(p->data);

					if (buffer.size() == 512){
						while (!buffer.empty()){
							output.write((char*)&buffer.front(), sizeof(char));
							buffer.pop_front();
						}
					}

				} else {
					while (!buffer.empty()){
						output.write((char*)&buffer.front(), sizeof(char));
						buffer.pop_front();
					}

					buffer.push_back(EOF);
				}

				p = tree;
			}
		}
	}

	output.close();
	input.close();
}

int main(int argc, char** argv)
{
	char filename[] = "input.bin";

	if (strcmp(getExtension(filename), "bin")){
		compress(filename);
	} else {
		decompress(filename);
	}

	return 0;
}