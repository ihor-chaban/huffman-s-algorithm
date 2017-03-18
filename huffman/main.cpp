/////////////////////////////////////////////////
//											   //
//	Implementation of data compressing and	   //
//	decompressing using Huffman's algorithm	   //
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
#define MAX_RANGE_OF_TREE unsigned int

using namespace std;

struct Node{
	int number;
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

		for (int i = 0; i < indent; i++){
			cout << " ";
		}

		cout << root->number;
		if (root->data){
			cout << "(" << root->data << ")";
		}
		cout << endl;

		if (root->left){
			printTree(root->left, indent + 3);
		}
	}
}

char* extension(char* input){
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
		if (first[i].data){
			second.push_back(1);

			for (int j = 7; j >= 0; j--){
				second.push_back((first[i].data >> j) & 1);
			}
		} else {
			second.push_back(0);
		}
	}

	return second;
}

Node* buildTree(char* filename){
	map <char, vector <bool> > table;
	map <char, int> count;
	char temp;

	fstream input(filename, ios_base::binary | ios_base::in);
	if (input){
		while (!input.eof()){
			input.read((char*)&temp, sizeof(char));
			if (temp != EOF){
				count[temp]++;
			}
		}
	}
	input.close();

	list <Node*> nodeList;
	for (map<char, int>::iterator itr = count.begin(); itr != count.end(); itr++){
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

		if (root->data){
			table[root->data] = code;
		}

		if (!code.empty()){
			code.pop_back();
		}
	}
}

void compress(char* filename){
	// getting source file extension
	char* ext = extension(filename);
	char ext_length = strlen(ext);

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
	fstream output("output.bin", ios_base::binary | ios_base::out | ios_base::trunc);

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

	// opening source file
	fstream input(filename, ios_base::binary | ios_base::in);

	// replacement of source data by its binary codes
	buffer = 0; c_buf = 0;
	char temp;
	while (!input.eof()){
		input.read((char*)&temp, sizeof(char));
		if (temp != EOF){
			for (vector <bool>::iterator i = table[temp].begin(); i != table[temp].end(); i++){
				buffer |= (*i) << (7 - c_buf++);
				if (c_buf == 8){
					output.write((char*)&buffer, sizeof(char));
					c_buf = 0;
					buffer = 0;
				}
			}
		}
	}
	if (c_buf){
		output.write((char*)&buffer, sizeof(char));
	}

	input.close();
	output.close();
}

void decompress(char* filename){
	/*Node* p = tree;
	int count = 0; char byte; bool b; int i = 0;
	while (out[i] != '\0'){
		byte = out[i];
		b = byte & (1 << (7 - count));
		if (b){
			p = p->right;
		}
		else {
			p = p->left;
		}

		if ((p->left == NULL) && (p->right == NULL)){
			cout << p->data;
			p = tree;
		}

		count++;

		if (count == 8){
			count = 0;
			byte = out[i++];
		}
	}*/
}

int main()
{
	compress("input.txt");
	return 0;
}