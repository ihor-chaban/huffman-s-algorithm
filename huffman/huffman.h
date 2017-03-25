/////////////////////////////////////////////////
//											   //
//	 Implementation of data compression and	   //
//	  decompression using Huffman algorithm	   //
//											   //
//			 Author - Ihor Chaban			   //
//											   //
/////////////////////////////////////////////////

#ifndef HUFFMAN_H
#define HUFFMAN_H
#define COMPRESSED_EXTENSION "huf"
#include <deque>
#include <vector>

class huffman
{
private:
	struct Node;
	struct MyCompare;

	char* filename;
	Node* root;
	std::deque <bool> tree_bool;

public:
	static huffman& getInstance();

	bool compress();
	bool decompress();
	bool toggle();

	char* getFilename();
	void setFilename(char*);

private:
	huffman();
	huffman(char*);
	huffman(const huffman &) = delete;
	huffman &operator=(const huffman &) = delete;
	~huffman();

	void buildTree();
	void restoreTree(char*, unsigned short int);
	void treeToBool();

	static void buildTable(Node*, std::vector <std::vector <bool> > &);
	static void deleteTree(Node* &);
	static inline bool isPeak(Node*);
	static inline bool getBit(char, unsigned char);
	static inline char setBit(char, bool, unsigned char);

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