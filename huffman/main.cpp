#include <iostream>
#include <list>
#include <map>
#include <vector>
using namespace std;

struct Node{
	int number;
	char letter;
	Node *left, *right;
};

struct MyCompare{
	bool operator () (Node* left, Node* right) const{
		return (left->number < right->number);
	}
};

void printTree(Node* root, int k = 0){
	if (root){
		if (root->right){
			printTree(root->right, k + 3);
		}

		for (int i = 0; i < k; i++){
			cout << " ";
		}

		cout << root->number;
		if (root->letter){
			cout << "(" << root->letter << ")";
		}
		cout << endl;
		
		if (root->left){
			printTree(root->left, k + 3);
		}
	}
}

vector <bool> code;
map <char, vector <bool> > table;

void buildTable(Node* root){
	if (root){
		if (root->left){
			code.push_back(0);
			buildTable(root->left);
		}

		if (root->right){
			code.push_back(1);
			buildTable(root->right);
		}

		if (root->letter){
			table[root->letter] = code;
		}

		if (!code.empty()){
			code.pop_back();
		}
	}
}

int main()
{
	char str[] = "it is my striiiiing!!!!";

	map <char, int> count;
	for (int i = 0; str[i] != '\0'; i++){
		count[str[i]]++;
	}

	list <Node*> nodeList;
	for (map<char, int>::iterator itr = count.begin(); itr != count.end(); itr++){
		Node *temp = new Node();
		temp->number = itr->second;
		temp->letter = itr->first;
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

	buildTable(nodeList.front());

	for (int i = 0; str[i] != '\0'; i++){
		for (auto i : table[str[i]])
			cout << i;
	}

	return 0;
}