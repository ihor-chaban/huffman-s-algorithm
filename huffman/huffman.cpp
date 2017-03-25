#include "huffman.h"
#include <algorithm>
#include <climits>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::fstream;
using std::string;
using std::deque;
using std::vector;
using std::ios_base;
using std::sort;
using std::endl;
using std::cerr;

struct huffman::Node{
	unsigned long long int number;
	char data;
	Node *left, *right;
};

struct huffman::MyCompare{
	bool operator () (Node* left, Node* right) const{
		return (left->number > right->number);
	}
};

huffman& huffman::getInstance()
{
	static huffman inst;
	return inst;
}

huffman::huffman(){
	filename = "";
	root = NULL;
}

huffman::~huffman(){
	if (root){
		deleteTree(root);
	}
}

char* huffman::getFilename(){
	return filename;
}

void huffman::setFilename(char* filename){
	this->filename = filename;
}

// case insensitive comparison
bool huffman::iCompare(string const &a, string const &b){
	if (a.length() == b.length()){
		unsigned int i = 0;
		while ((tolower(a[i]) == tolower(b[i])) && (i < a.length())){
			i++;
		}
		if (i == a.length()){
			return true;
		}
	}
	return false;
}

inline bool huffman::getBit(char var, unsigned char pos){
	return (((var) & (1 << pos)) != 0);
}

inline char huffman::setBit(char var, bool value, unsigned char pos){
	return ((var) | (value << pos));
}

inline bool huffman::isPeak(Node* node){
	return ((node->left == NULL) && (node->right == NULL));
}

void huffman::deleteTree(Node* &node){
	if (node){
		deleteTree(node->left);
		deleteTree(node->right);
		delete node;
		node = NULL;
	}
}

string huffman::addExtension(string input, string extension){
	return (input + '.' + extension);
}

string huffman::changeExtension(string input, string extension){
	return addExtension(removeExtension(input), extension);
}

string huffman::getExtension(string input, char delim){
	if (input.find(delim) != string::npos){
		return input.erase(0, input.find_last_of(delim) + 1);
	} else {
		return "";
	}
}

string huffman::removeExtension(string input){
	if (input.find('.') != string::npos){
		return input.erase(input.find_last_of('.'), input.length() - input.find_last_of('.'));
	} else {
		return input;
	}
}

void huffman::buildTree(){
	// counting the repetitions of each byte
	vector <unsigned long long int> count(UCHAR_MAX + 1);
	fstream input(filename, ios_base::in | ios_base::binary);
	if (input.is_open()){
		char temp_byte;
		while (input.read(&temp_byte, sizeof(char))){
			count[unsigned char(temp_byte)]++;
		}
		count[unsigned char(EOF)]++;
	} else {
		return;
	}
	input.close();

	// building binary tree from count
	// creating and pushing all nodes
	vector <Node*> nodes;
	for (unsigned int i = 0; i < count.size(); i++){
		if (count[i]){
			Node *temp_node = new Node();
			temp_node->data = i;
			temp_node->number = count[i];
			nodes.push_back(temp_node);
		}
	}

	// while not left only root assign every node its children
	while (nodes.size() != 1){
		sort(nodes.begin(), nodes.end(), MyCompare());
		Node* temp_node = new Node();
		temp_node->left = nodes.back();
		temp_node->number = nodes.back()->number;
		nodes.pop_back();
		temp_node->right = nodes.back();
		temp_node->number += nodes.back()->number;
		nodes.pop_back();
		nodes.push_back(temp_node);
	}

	root = nodes.front();
}

void huffman::restoreTree(char* input, unsigned short int size){
	// converting bytes input to bits
	deque <bool> tree_bool_temp;
	for (int i = 0; i < size; i++){
		for (int j = 7; j >= 0; j--){
			tree_bool_temp.push_back(getBit(input[i], j));
		}
	}

	// restoring binary tree from bits
	// pushing initial nodes of tree
	Node* temp_root = NULL;
	deque <Node*> current, next;
	if (!tree_bool_temp.empty() && !tree_bool_temp.front()){
		Node* temp_node = new Node();
		Node* temp_left = new Node();
		Node* temp_right = new Node();

		temp_node->left = temp_left;
		temp_node->right = temp_right;

		current.push_back(temp_left);
		current.push_back(temp_right);

		tree_bool_temp.pop_front();
		temp_root = temp_node;
	}

	// while tree is not complete and input not ends assign every node its children
	while (!current.empty() && !tree_bool_temp.empty()){
		while (!current.empty() && !tree_bool_temp.empty()){
			if (!tree_bool_temp.front()){
				Node* temp_left = new Node();
				Node* temp_right = new Node();

				current.front()->left = temp_left;
				current.front()->right = temp_right;

				next.push_back(temp_left);
				next.push_back(temp_right);

				current.pop_front();
				tree_bool_temp.pop_front();
			} else {
				current.front()->data = 0;
				tree_bool_temp.pop_front();
				for (int i = 7; i >= 0; i--){
					current.front()->data = setBit(current.front()->data, tree_bool_temp.front(), i);
					tree_bool_temp.pop_front();
				}
				current.pop_front();
			}
		}
		swap(current, next);
	}

	// if input ends but tree is not complete
	if (!current.empty() && tree_bool_temp.empty()){
		root = NULL;
	} else {
		root = temp_root;
	}
}

// building binary codes map for each byte
void huffman::buildTable(Node* node, vector <vector <bool> > &table){
	static vector <bool> code;
	if (node){
		if (node->left){
			code.push_back(0);
			buildTable(node->left, table);
		}

		if (node->right){
			code.push_back(1);
			buildTable(node->right, table);
		}

		if (isPeak(node)){
			table[unsigned char(node->data)] = code;
		}

		if (!code.empty()){
			code.pop_back();
		}
	}
}

// converting binary tree to binary code
void huffman::treeToBool(){
	// pushing tree nodes by levels and left-to-right order
	deque <Node> first;
	first.push_back(*root);
	int start_index = 0;
	for (int i = start_index; i != first.size(); i++){
		if (first[i].left){
			first.push_back(*first[i].left);
		}
		if (first[i].right){
			first.push_back(*first[i].right);
		}
		if (first[i].left || first[i].right){
			start_index++;
		}
	}

	// converting first deque to binary code
	// if node - push 0, if peak - push 1 and binary code of its byte
	deque <bool> second;
	for (unsigned int i = 0; i < first.size(); i++){
		if (isPeak(&first[i])){
			second.push_back(1);
			for (int j = 7; j >= 0; j--){
				second.push_back(getBit(first[i].data, j));
			}
		} else {
			second.push_back(0);
		}
	}

	tree_bool = second;
}

bool huffman::compress(){
	// opening input
	fstream input(filename, ios_base::in | ios_base::binary);
	if (!input.is_open()){
		throw exception(filename, "does not exist!");
		return 0;
	}

	// getting input file extension
	string ext = getExtension(filename, '.');
	char ext_length = ext.length();
	string out_filename = addExtension(filename, COMPRESSED_EXTENSION);

	// building binary tree from input file
	buildTree();

	// building map of bytes and its binary codes
	vector <vector <bool> > table(UCHAR_MAX + 1);
	buildTable(root, table);

	// converting binary tree into binary code
	treeToBool();

	deleteTree(root);

	// converting size of binary tree code from bits to bytes
	unsigned short int tree_size = static_cast <unsigned short int> (ceil(tree_bool.size() / 8.0));

	// opening output if not already exists
	fstream output(out_filename);
	if (!output.is_open()){
		output.open(out_filename, ios_base::out | ios_base::binary);
	} else {
		throw exception(filename, "has already compressed!");
		return 0;
	}

	// writing input file extension and its length
	output.write((char*)&ext_length, sizeof(char));
	for (int i = 0; i < ext_length; i++){
		output.write((char*)&ext[i], sizeof(char));
	}

	// writing binary tree code and its size
	output.write((char*)&tree_size, sizeof(unsigned short int));
	char buffer = 0, c_buf = 0;
	for (unsigned int i = 0; i < tree_bool.size(); i++){
		buffer = setBit(buffer, tree_bool[i], 7 - c_buf++);
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
	char temp_byte;

	// adding EOF marker at start of encoded data
	temp_byte = EOF;
	for (unsigned int i = 0; i < table[unsigned char(temp_byte)].size(); i++){
		buffer = setBit(buffer, table[unsigned char(temp_byte)][i], 7 - c_buf++);
		if (c_buf == 8){
			output.write((char*)&buffer, sizeof(char));
			c_buf = 0;
			buffer = 0;
		}
	}

	// replacing source data by its binary codes
	while (input.read(&temp_byte, sizeof(char))){
		for (unsigned int i = 0; i < table[unsigned char(temp_byte)].size(); i++){
			buffer = setBit(buffer, table[unsigned char(temp_byte)][i], 7 - c_buf++);
			if (c_buf == 8){
				output.write((char*)&buffer, sizeof(char));
				c_buf = 0;
				buffer = 0;
			}
		}
	}

	// adding EOF marker in end of encoded data
	temp_byte = EOF;
	for (unsigned int i = 0; i < table[unsigned char(temp_byte)].size(); i++){
		buffer = setBit(buffer, table[unsigned char(temp_byte)][i], 7 - c_buf++);
		if (c_buf == 8){
			output.write((char*)&buffer, sizeof(char));
			c_buf = 0;
			buffer = 0;
		}
	}

	// writing last incomplete byte if exists
	if (c_buf){
		while(c_buf < 8){
			buffer = setBit(buffer, !table[unsigned char(EOF)][0], 7 - c_buf++);
		}
		output.write((char*)&buffer, sizeof(char));
	}

	output.close();
	input.close();

	return 1;
}

bool huffman::decompress(){
	// opening input
	fstream input(filename, ios_base::in | ios_base::binary);
	if (!input.is_open()){
		throw exception(filename, "does not exist!");
		return 0;
	}

	// getting source extension and setting output filename
	char ext_length;
	char* ext = NULL;
	string out_filename;
	input.read(&ext_length, sizeof(char));

	// if source file has extension
	if (ext_length > 0){
		ext = new char[ext_length + 1];
		input.read(ext, sizeof(char)* ext_length);
		ext[ext_length] = '\0';
		if (iCompare(getExtension(removeExtension(filename), '.'), ext)){
			out_filename = removeExtension(filename);
		} else {
			out_filename = changeExtension(filename, ext);
		}
		delete[] ext;
		ext = NULL;
	} else {
		out_filename = filename;
		while (getExtension(out_filename, '.').length()){
			out_filename = removeExtension(out_filename);
		}
	}

	// reading size of binary tree code
	unsigned short int tree_size;
	input.read((char*)&tree_size, sizeof(unsigned short int));

	// valid binary tree code size can't be more than 320 bytes
	if (tree_size > 320){
		throw exception(filename, "is not a valid archive!");
		return 0;
	}

	// restoring binary tree from binary code
	char* temp_bytes = new char[tree_size];
	input.read(temp_bytes, sizeof(char)* tree_size);
	restoreTree(temp_bytes, tree_size);
	delete[] temp_bytes;
	temp_bytes = NULL;

	// if tree is empty after restoring of input ends
	if (!root || input.eof()){
		throw exception(filename, "is not a valid archive!");
		return 0;
	}

	// checking if first byte is EOF marker
	deque <char> buffer;
	Node* p = root;
	char byte;
	bool bit = false;
	bool first_peak = true;
	while (first_peak && input.read(&byte, sizeof(char))){
		for (int i = 7; i >= 0; i--){
			bit = getBit(byte, i);
			if (bit){
				p = p->right;
			} else {
				p = p->left;
			}
			if (isPeak(p)){
				if ((p->data != EOF) && (first_peak)){
					throw exception(filename, "is not a valid archive!");
					return 0;
				} else {
					if (!first_peak){
						buffer.push_back(p->data);
					}
					first_peak = false;
					p = root;
				}
			}
		}
	}

	// opening output if not already exists
	fstream output(out_filename);
	if (!output.is_open()){
		output.open(out_filename, ios_base::out | ios_base::binary);
	} else {
		throw exception(filename, "has already extracted!");
		return 0;
	}

	// restoring source data
	while (input.read(&byte, sizeof(char))){
		for (int i = 7; i >= 0; i--){
			bit = getBit(byte, i);
			if (bit){
				p = p->right;
			} else {
				p = p->left;
			}
			if (isPeak(p)){
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
				p = root;
			}
		}
	}

	deleteTree(root);
	output.close();
	input.close();

	return 1;
}

// if not compressed - compress, if compressed - decompress
bool huffman::toggle(){
	if (iCompare(getExtension(filename, '.'), COMPRESSED_EXTENSION)){
		return decompress();
	} else {
		return compress();
	}
}

huffman::exception::exception(){
	filename = "";
	message = "";
}

huffman::exception::exception(char* filename, char* message){
	this->filename = filename;
	this->message = message;
}

void huffman::exception::what(){
	cerr << filename << " " << message << endl;
}

void huffman::exception::showFilename(){
	cerr << filename << endl;
}

void huffman::exception::showMessage(){
	cerr << message << endl;
}