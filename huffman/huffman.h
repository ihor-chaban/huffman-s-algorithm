/////////////////////////////////////////////////
//											   //
//	Implementation of data compression and	   //
//	decompression using Huffman's algorithm	   //
//											   //
//			Author - Ihor Chaban			   //
//											   //
/////////////////////////////////////////////////

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <map>
#include <vector>

// file extension after compression
#define COMPRESSED_EXTENSION "bin"

// max range of binary tree array size in bytes
#define MAX_RANGE_OF_TREE unsigned short int

class huffman
{
private:
	struct Node;
	struct MyCompare;

	char* filename;
	Node* tree;
	std::vector <bool> tree_bin;

public:
	huffman();
	huffman(const huffman &);
	huffman(char*);
	huffman &operator=(const huffman &);
	~huffman();

	char* getFilename();
	void setFilename(char*);

	void compress();
	void decompress();
	void toggle();

private:
	static bool iCompare(std::string const &, std::string const &);
	static inline bool isPeak(Node*);

	static void printTree(Node*, unsigned int);
	static void deleteTree(Node* &);

	static std::string addExtension(std::string, std::string);
	static std::string changeExtension(std::string, std::string);
	static std::string getExtension(std::string, char);
	static std::string removeExtension(std::string);

	void buildTree();
	static Node* restoreTree(char*, MAX_RANGE_OF_TREE);
	static void buildTable(Node*, std::map <char, std::vector <bool> > &);
	void treeToBin();
};

#endif HUFFMAN_H