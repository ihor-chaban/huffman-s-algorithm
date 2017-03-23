#include <iostream>
#include <ctime> 
#include <math.h>
#include <string>
#include "huffman.h"
using namespace std;

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

string getExtension(string input, char delim){
	if (input.find(delim) != string::npos) {
		return input.erase(0, input.find_last_of(delim) + 1);
	}
	else {
		return "";
	}
}

//void huffman::printTree(Node* node, unsigned int indent = 0){
//	if (node){
//		if (node->right){
//			printTree(node->right, indent + 3);
//		}
//
//		for (unsigned int i = 0; i < indent; i++){
//			std::cout << " ";
//		}
//
//		std::cout << node->number;
//		if (isPeak(node)){
//			std::cout << "(" << node->data << ")";
//		}
//		std::cout << std::endl;
//
//		if (node->left){
//			printTree(node->left, indent + 3);
//		}
//	}
//}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");
	huffman archiver;

	bool archive;
	unsigned long int to_archive = 0, archived = 0, to_extract = 0, extracted = 0;
	unsigned long int start_time = clock(), end_time, result_time;

	/*if (argc > 1){
		for (int i = 1; i < argc; i++){
			archiver.setFilename(argv[i]);

			cout << ">> ";
			if (argc > 2){
				cout << "(" << i << "/" << argc - 1 << ") ";
			}
			if (iCompare(getExtension(argv[i], '.'), COMPRESSED_EXTENSION)){
				archive = false;
				to_extract++;
				cout << "Extracting ";
			} else {
				archive = true;
				to_archive++;
				cout << "Archiving ";
			}
			cout << "\"" << getExtension(argv[i], '\\') << "\" ........ ";

			try {
				if (archiver.toggle()){
					(archive) ? archived++ : extracted++;
					cout << "done." << endl;
				}
			} catch (huffman::exception e){
				e.showMessage();
			}
		}

		if (to_archive){
			cout << ">> Files to archive: " << to_archive << ", Successfully: " << archived;
			if (to_archive != archived){
				cout << ", Bad: " << to_archive - archived;
			}
			cout << "." << endl;
		}
		if (to_extract){
			cout << ">> Files to extract: " << to_extract << ", Successfully: " << extracted;
			if (to_extract != extracted){
				cout << ", Bad: " << to_extract - extracted;
			}
			cout << "." << endl;
		}
	}*/

	end_time = clock();
	result_time = end_time - start_time;
	unsigned long int hour, min, sec;
	hour = result_time / 3600000;
	result_time -= hour * 3600000;
	min = result_time / 60000;
	result_time -= min * 60000;
	sec = result_time / 1000;
	result_time -= sec * 1000;

	if (hour){
		cout << hour << " hours ";
	};
	if (min){
		cout << min << " minutes ";
	};
	if (sec){
		cout << sec << " seconds ";
	};
	if (result_time){
		cout << result_time << " miliseconds";
	};
	cout << endl;

	std::system("pause");
	return 0;
}