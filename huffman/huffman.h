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
#define COMPRESSED_EXTENSION "huf"
#include <deque>
#include <map>
#include <vector>

class huffman
{
private:
	struct Node;
	struct MyCompare;

	char* filename;
	Node* tree;
	std::deque <bool> tree_bin;

public:
	huffman();
	huffman(const huffman &);
	huffman(char*);
	huffman &operator=(const huffman &);
	~huffman();

	bool compress();
	bool decompress();
	bool toggle();

	char* getFilename();
	void setFilename(char*);

private:
	void buildTree();
	void restoreTree(char*, unsigned short int);
	void treeToBin();

	static void buildTable(Node*, std::map <char, std::vector <bool> > &);
	
	static inline bool isPeak(Node*);

	static void printTree(Node*, unsigned int);
	static void deleteTree(Node* &);

	static bool iCompare(std::string const &, std::string const &);
	static std::string addExtension(std::string, std::string);
	static std::string changeExtension(std::string, std::string);
	static std::string getExtension(std::string, char);
	static std::string removeExtension(std::string);

public:
	class exception{
	private:
		char* filename;
		char* message;

	public:
		exception();
		exception(char*, char*);
		void what();
		void showFilename();
		void showMessage();
	};
};

#endif HUFFMAN_H