#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <map>
#include <queue>
#include <vector>
#include "huffman.h"

struct huffman::Node{
	unsigned long int number;
	char data;
	Node *left, *right;
};

struct huffman::MyCompare{
	bool operator () (Node* left, Node* right) const{
		return (left->number < right->number);
	}
};

huffman::huffman(){
	filename = "";
	tree = NULL;
}

huffman::huffman(const huffman &obj){
	filename = obj.filename;
	tree = obj.tree;
	tree_bin = obj.tree_bin;
}

huffman::huffman(char* filename){
	this->filename = filename;
	tree = NULL;
}

huffman& huffman::operator=(const huffman &obj){
	filename = obj.filename;
	tree = obj.tree;
	tree_bin = obj.tree_bin;
	return *this;
}

huffman::~huffman(){
	if (tree){
		deleteTree(tree);
	}
}

char* huffman::getFilename(){
	return filename;
}

void huffman::setFilename(char* filename){
	this->filename = filename;
}

bool huffman::iCompare(std::string const &a, std::string const &b){
	if (a.length() == b.length()) {
		unsigned int i = 0;
		for (; (i < a.length()) && (tolower(a[i]) == tolower(b[i])); i++){}
		if (i == a.length()){
			return true;
		}
	}
	return false;
}

inline bool huffman::isPeak(Node* root){
	return (root->left == NULL && root->right == NULL);
}

void huffman::printTree(Node* tree, unsigned int indent = 0){
	if (tree){
		if (tree->right){

			printTree(tree->right, indent + 3);
		}

		for (unsigned int i = 0; i < indent; i++){
			std::cout << " ";
		}

		std::cout << tree->number;
		if (isPeak(tree)){
			std::cout << "(" << tree->data << ")";
		}
		std::cout << std::endl;

		if (tree->left){
			printTree(tree->left, indent + 3);
		}
	}
}

void huffman::deleteTree(Node* &tree){
	if (tree){
		deleteTree(tree->left);
		deleteTree(tree->right);
		delete tree;
		tree = NULL;
	}
}

std::string huffman::addExtension(std::string input, std::string extension){
	return input + '.' + extension;
}

std::string huffman::changeExtension(std::string input, std::string extension){
	return huffman::addExtension(huffman::removeExtension(input), extension);
}

std::string huffman::getExtension(std::string input, char delim){
	if (input.find(delim) != std::string::npos) {
		return input.erase(0, input.find_last_of(delim) + 1);
	}
	else {
		return "";
	}
}

std::string huffman::removeExtension(std::string input){
	if (input.find('.') != std::string::npos) {
		return input.erase(input.find_last_of('.'), input.length() - input.find_last_of('.'));
	}
	else {
		return input;
	}
}

void huffman::buildTree(){
	std::map <char, std::vector <bool> > table;
	std::map <char, unsigned long int> count;
	char temp;

	std::fstream input(filename, std::ios_base::in | std::ios_base::binary);
	if (input.is_open()){
		while (input.read(&temp, sizeof(char))){
			count[temp]++;
		}
		count[EOF]++;
	}
	else {
		return;
	}
	input.close();

	std::deque <Node*> nodes;
	for (std::map<char, unsigned long int>::iterator itr = count.begin(); itr != count.end(); itr++){
		Node *temp = new Node();
		temp->number = itr->second;
		temp->data = itr->first;
		nodes.push_front(temp);
	}

	while (nodes.size() != 1){
		std::sort(nodes.begin(), nodes.end(), MyCompare());
		Node* temp = new Node();
		temp->left = nodes.front();
		temp->number = nodes.front()->number;
		nodes.pop_front();
		temp->right = nodes.front();
		temp->number += nodes.front()->number;
		nodes.pop_front();
		nodes.push_front(temp);
	}

	tree = nodes.front();
}

void huffman::restoreTree(char* input, unsigned short int size){
	std::deque <bool> tree_bin;
	for (unsigned int i = 0; i < size; i++){
		for (unsigned int j = 0; j < 8; j++){
			tree_bin.push_back((input[i] & (1 << (7 - j))) != 0);
		}
	}

	Node* root = NULL;
	std::deque <Node*> current, next;
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

	while (!current.empty() && !tree_bin.empty()){
		while (!current.empty() && !tree_bin.empty()){
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

	if (!current.empty() && tree_bin.empty()){
		tree = NULL;
	}

	tree = root;
}

void huffman::buildTable(Node* root, std::map <char, std::vector <bool> > &table){
	static std::vector <bool> code;
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

void huffman::treeToBin(){
	std::vector <Node> first;
	first.push_back(*tree);
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

	std::vector <bool> second;
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

	tree_bin = second;
}

bool huffman::compress(){
	// opening source file
	std::fstream input(filename, std::ios_base::in | std::ios_base::binary);
	if (!input.is_open()){
		throw exception(filename, "does not exist!");
		return 0;
	}

	// getting source file extension
	std::string ext = getExtension(filename, '.');
	char ext_length = ext.length();
	std::string out_filename = addExtension(filename, COMPRESSED_EXTENSION);

	// building binary tree from input file
	buildTree();

	// building map of chars and its binary code
	std::map <char, std::vector <bool> > table;
	buildTable(tree, table);

	// converting binary tree into binary array
	treeToBin();

	deleteTree(tree);

	// converting size of binary tree array from bits to bytes
	unsigned short int tree_size = static_cast <unsigned short int> (ceil(tree_bin.size() / 8.0));

	// opening binary output
	std::fstream output(out_filename);
	if (!output.is_open()){
		output.open(out_filename, std::ios_base::out | std::ios_base::binary);
	}
	else {
		throw exception(filename, "has already compressed!");
		return 0;
	}

	// writing source file extension and its length
	output.write((char*)&ext_length, sizeof(char));
	for (int i = 0; i < ext_length; i++){
		output.write((char*)&ext[i], sizeof(char));
	}

	// writing binary tree array and its size
	output.write((char*)&tree_size, sizeof(unsigned short int));
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
	for (std::vector <bool>::iterator i = table[temp].begin(); i != table[temp].end(); i++){
		buffer |= (*i) << (7 - c_buf++);
		if (c_buf == 8){
			output.write((char*)&buffer, sizeof(char));
			c_buf = 0;
			buffer = 0;
		}
	}

	// replacement of source data by its binary codes
	while (input.read(&temp, sizeof(char))){
		for (std::vector <bool>::iterator i = table[temp].begin(); i != table[temp].end(); i++){
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
	for (std::vector <bool>::iterator i = table[temp].begin(); i != table[temp].end(); i++){
		buffer |= (*i) << (7 - c_buf++);
		if (c_buf == 8){
			output.write((char*)&buffer, sizeof(char));
			c_buf = 0;
			buffer = 0;
		}
	}

	// writing last incomplete byte if exists
	if (c_buf){
		for (; c_buf < 8; c_buf++){
			buffer |= (!table[EOF][0]) << (7 - c_buf);
		}
		output.write((char*)&buffer, sizeof(char));
	}

	output.close();
	input.close();

	return 1;
}

bool huffman::decompress(){
	// opening source file
	std::fstream input(filename, std::ios_base::in | std::ios_base::binary);
	if (!input.is_open()){
		throw exception(filename, "does not exist!");
		return 0;
	}

	// getting source extension and setting output filename
	char ext_length;
	char* ext = NULL;
	std::string out_filename;
	input.read(&ext_length, sizeof(char));

	// if source file has extension
	if (ext_length > 0){
		ext = new char[ext_length + 1];
		input.read(ext, sizeof(char)* ext_length);

		ext[ext_length] = '\0';

		if (iCompare(getExtension(removeExtension(filename), '.'), ext)){
			out_filename = removeExtension(filename);
		}
		else {
			out_filename = changeExtension(filename, ext);
		}
	}
	else {
		out_filename = filename;
		while (getExtension(out_filename, '.').length() != 0){
			out_filename = removeExtension(out_filename);
		}
	}

	if (ext){
		delete[] ext;
		ext = NULL;
	}

	// reading size of binary tree array
	unsigned short int tree_size;
	input.read((char*)&tree_size, sizeof(unsigned short int));
	if (tree_size > 320){
		throw exception(filename, "is not a valid archive!");
		return 0;
	}

	// restoring binary tree from array
	char* temp = new char[tree_size];
	input.read(temp, sizeof(char)* tree_size);
	restoreTree(temp, tree_size);
	delete[] temp;
	temp = NULL;

	if (!tree){
		throw exception(filename, "is not a valid archive!");
		return 0;
	}

	Node* p = tree;
	char byte;
	bool bit = false;

	// checking if file is valid archive
	bool first_peak = true;
	std::queue <char> buffer;
	if (input.eof()){
		throw exception(filename, "is not a valid archive!");
		return 0;
	}
	while (first_peak && input.read(&byte, sizeof(char))){
		for (int i = 0; i < 8; i++){
			bit = ((byte & (1 << (7 - i))) != 0);
			if (bit == 0){
				p = p->left;
			}
			if (bit == 1){
				p = p->right;
			}
			if (isPeak(p)){
				if ((p->data != EOF) && (first_peak)){
					throw exception(filename, "is not a valid archive!");
					return 0;
				}
				else {
					if (!first_peak){
						buffer.push(p->data);
					}
					first_peak = false;
					p = tree;
				}
			}
		}
	}

	// checking if source file already exists
	std::fstream output(out_filename);
	if (!output.is_open()){
		output.open(out_filename, std::ios_base::out | std::ios_base::binary);
	}
	else {
		throw exception(filename, "has already extracted!");
		return 0;
	}

	// restoring source data
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
				}
				else {
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

	return 1;
}

bool huffman::toggle(){
	if (iCompare(getExtension(filename, '.'), COMPRESSED_EXTENSION)){
		return decompress();
	}
	else {
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
	std::cerr << filename << " " << message << std::endl;
}

void huffman::exception::showFilename(){
	std::cerr << filename << std::endl;
}

void huffman::exception::showMessage(){
	std::cerr << message << std::endl;
}