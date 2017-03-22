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
#include <map>
#include <vector>

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

	bool compress();
	bool decompress();
	bool toggle();

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
	void restoreTree(char*, unsigned short int);
	static void buildTable(Node*, std::map <char, std::vector <bool> > &);
	void treeToBin();

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