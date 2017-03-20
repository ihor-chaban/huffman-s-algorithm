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
#include <string>
#include <list>
#include <map>
#include <queue>
#include <vector>

// file extension after compressing
#define COMPRESSED_EXTENSION "bin"

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

inline bool isPeak(Node* root){
	return (root->left == NULL && root->right == NULL);
}

void printTree(Node* root, unsigned int indent = 0){
	if (root){
		if (root->right){
			printTree(root->right, indent + 3);
		}

		for (unsigned int i = 0; i < indent; i++){
			cout << " ";
		}

		cout << root->number;
		if (isPeak(root)){
			cout << "(" << root->data << ")";
		}
		cout << endl;

		if (root->left){
			printTree(root->left, indent + 3);
		}
	}
}

void deleteTree(Node* &root){
	if (root){
		deleteTree(root->left);
		deleteTree(root->right);
		delete root;
	}
}

string getExtension(string input){
	if (input.find('.') != string::npos) {
		return input.erase(0, input.find_last_of('.') + 1);
	} else {
		return "";
	}
}

string removeExtension(string input){
	if (input.find('.') != string::npos) {
		return input.erase(input.find_last_of('.'), input.length() - input.find_last_of('.'));
	} else {
		return input;
	}
}

string addExtension(string input, string extension){
	return input + '.' + extension;
}

string changeExtension(string input, string extension){
	return addExtension(removeExtension(input), extension);
}

bool iCompare(string const &a, string const &b){
	if (a.length() == b.length()) {
		unsigned int i = 0;
		for (; (i < a.length()) && (tolower(a[i]) == tolower(b[i])); i++){}
		if (i == a.length()){
			return true;
		}
	}

	return false;
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
		if (isPeak(&first[i])){
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

	list <Node*> nodes;
	for (map<char, unsigned long int>::iterator itr = count.begin(); itr != count.end(); itr++){
		Node *temp = new Node();
		temp->number = itr->second;
		temp->data = itr->first;
		nodes.push_back(temp);
	}

	while (nodes.size() != 1){
		nodes.sort(MyCompare());
		Node* temp = new Node();
		temp->left = nodes.front();
		temp->number = nodes.front()->number;
		nodes.pop_front();
		temp->right = nodes.front();
		temp->number += nodes.front()->number;
		nodes.pop_front();
		nodes.push_front(temp);
	}

	return nodes.front();
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

		if (isPeak(root)){
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
			tree_bin.push_back((input[i] & (1 << (7 - j))) != 0);
		}
	}

	Node* root = NULL;
	list <Node*> current, next;
	if (!tree_bin.empty() && !tree_bin.front()){
		Node* temp = new Node();

		Node* tempL = new Node();
		Node* tempR = new Node();

		temp->left = tempL;
		temp->right = tempR;

		current.push_back(tempL);
		current.push_back(tempR);

		tree_bin.pop_front();
		root = temp;
	}

	while (!current.empty()){
		while (!current.empty()){
			if (!tree_bin.front()){
				Node* tempL = new Node();
				Node* tempR = new Node();

				current.front()->left = tempL;
				current.front()->right = tempR;

				next.push_back(tempL);
				next.push_back(tempR);

				current.pop_front();
				tree_bin.pop_front();
			}
			else {
				current.front()->data = 0;
				tree_bin.pop_front();
				for (int i = 0; i < 8; i++){
					current.front()->data |= tree_bin.front() << (7 - i);
					tree_bin.pop_front();
				}
				current.pop_front();
			}
		}
		swap(current, next);
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
	string ext = getExtension(filename);
	char ext_length = ext.length();
	string out_filename = addExtension(filename, COMPRESSED_EXTENSION);

	// building binary tree from input file
	Node* tree = buildTree(filename);

	// building map of chars and its binary code
	map <char, vector <bool> > table;
	buildTable(tree, table);

	// converting binary tree into binary array
	vector <bool> tree_bin = treeToBin(tree);

	deleteTree(tree);

	// converting size of binary tree array from bits to bytes
	MAX_RANGE_OF_TREE tree_size = static_cast <MAX_RANGE_OF_TREE> (ceil(tree_bin.size() / 8.0));

	// opening binary output
	fstream output(out_filename);
	if (!output.is_open()){
		output.open(out_filename, ios_base::out | ios_base::binary);
	} else {
		cout << "File " << filename << " already compressed!" << endl;
		return;
	}

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

	
	buffer = 0; c_buf = 0;
	char temp;

	// adding EOF marker at start of encoded data
	temp = EOF;
	for (vector <bool>::iterator i = table[temp].begin(); i != table[temp].end(); i++){
		buffer |= (*i) << (7 - c_buf++);
		if (c_buf == 8){
			output.write((char*)&buffer, sizeof(char));
			c_buf = 0;
			buffer = 0;
		}
	}

	// replacement of source data by its binary codes
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

	// adding EOF marker in end of encoded data
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
	char* ext = NULL;
	string out_filename;
	input.read(&ext_length, sizeof(char));

	// if source file has extension
	if (ext_length > 0){
		ext = new char[ext_length + 1];
		input.read(ext, sizeof(char) * ext_length);

		ext[ext_length] = '\0';

		if (iCompare(getExtension(removeExtension(filename)), ext)){
			out_filename = removeExtension(filename);
		} else {
			out_filename = changeExtension(filename, ext);
		}
	} else {
		out_filename = filename;
		while (getExtension(out_filename).length() != 0){
			out_filename = removeExtension(out_filename);
		}
	}

	if (ext){
		delete[] ext;
	}
	
	// reading size of binary tree array
	MAX_RANGE_OF_TREE tree_size;
	input.read((char*)&tree_size, sizeof(MAX_RANGE_OF_TREE));
	
	// restoring binary tree from array
	char* temp = new char[tree_size];
	input.read(temp, sizeof(char)* tree_size);
	Node* tree = restoreTree(temp, tree_size);
	delete[]temp;

	Node* p = tree;
	char byte;
	bool bit = false;

	// checking if file is valid archive
	if (!input.read(&byte, sizeof(char))){
		cout << "File " << filename << " is not a valid archive!" << endl;
		return;
	}
	for (int i = 0; i < 8; i++){
		bit = ((byte & (1 << (7 - i))) != 0);
		if (bit == 0){
			p = p->left;
		}
		if (bit == 1){
			p = p->right;
		}
		if (isPeak(p)){
			if (p->data != EOF){
				cout << "File " << filename << " is not a valid archive!" << endl;
				return;
			} else {
				p = tree;
			}
		}
	}

	// checking if source file already exists
	fstream output(out_filename);
	if (!output.is_open()){
		output.open(out_filename, ios_base::out | ios_base::binary);
	}
	else {
		cout << "File " << filename << " already extracted!" << endl;
		return;
	}

	// restoring source data
	queue <char> buffer;
	while (input.read(&byte, sizeof(char))){
		for (int i = 0; i < 8; i++){
			bit = ((byte & (1 << (7 - i))) != 0);

			if (bit == 0){
				p = p->left;
			}
			if (bit == 1){
				p = p->right;
			}
			if (isPeak(p)){
				if (p->data != EOF){
					buffer.push(p->data);
					if (buffer.size() == 512){
						while (!buffer.empty()){
							output.write((char*)&buffer.front(), sizeof(char));
							buffer.pop();
						}
					}
				} else {
					while (!buffer.empty()){
						output.write((char*)&buffer.front(), sizeof(char));
						buffer.pop();
					}
					buffer.push(EOF);
				}
				p = tree;
			}
		}
	}

	deleteTree(tree);
	output.close();
	input.close();
}

int main(int argc, char** argv)
{
	char filename[] = "input.exe.bin";

	if (iCompare(getExtension(filename), COMPRESSED_EXTENSION)){
		decompress(filename);
	} else {
		compress(filename);
	}

	return 0;
}