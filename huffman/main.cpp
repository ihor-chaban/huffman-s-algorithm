#include <iostream>
#include <string>
#include "huffman.h"
using namespace std;

string getExtension(string input, char delim){
	if (input.find(delim) != string::npos) {
		return input.erase(0, input.find_last_of(delim) + 1);
	}
	else {
		return "";
	}
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");
	huffman archiver;

	if (argc > 1){
		for (int i = 1; i < argc; i++){
			archiver.setFilename(argv[i]);
			cout << "File (" << i << "/" << argc - 1 << ") "
				<< getExtension(argv[i], '\\') << " ........ ";
			try {
				archiver.toggle();
			} catch (huffman::exception e){
				e.showMessage();
			}
		}
	}

	system("pause");
	return 0;
}