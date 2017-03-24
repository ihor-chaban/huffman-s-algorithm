#include <iostream>
#include <ctime> 
#include <string>
#include "huffman.h"
using namespace std;

bool iCompare(string const &a, string const &b){
	if (a.length() == b.length()){
		unsigned int i = 0;
		for (; (i < a.length()) && (tolower(a[i]) == tolower(b[i])); i++){}
		if (i == a.length()){
			return true;
		}
	}
	return false;
}

string getExtension(string input, char delim){
	if (input.find(delim) != string::npos){
		return input.erase(0, input.find_last_of(delim) + 1);
	} else {
		return "";
	}
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");
	huffman archiver;

	bool archiving;
	unsigned long int to_archive = 0, archived = 0, to_extract = 0, extracted = 0;
	unsigned long int start_time, end_time, result_time;
	
	if (argc > 1){
		start_time = clock();
		for (int i = 1; i < argc; i++){
			archiver.setFilename(argv[i]);

			cout << ">> ";
			if (argc > 2){
				cout << "(" << i << "/" << argc - 1 << ") ";
			}
			if (iCompare(getExtension(argv[i], '.'), COMPRESSED_EXTENSION)){
				archiving = false;
				to_extract++;
				cout << "Extracting ";
			} else {
				archiving = true;
				to_archive++;
				cout << "Archiving ";
			}
			cout << "\"" << getExtension(argv[i], '\\') << "\" ........ ";

			try {
				if (archiver.toggle()){
					(archiving) ? archived++ : extracted++;
					cout << "done." << endl;
				}
			} catch (huffman::exception e){
				e.showMessage();
			}
		}
		end_time = clock();
		cout << ">> " << endl;

		if (argc > 2){
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
		}

		result_time = end_time - start_time;
		unsigned long int hour, min, sec;
		hour = result_time / 3600000;
		result_time -= hour * 3600000;
		min = result_time / 60000;
		result_time -= min * 60000;
		sec = result_time / 1000;
		result_time -= sec * 1000;

		cout << ">> Finished in ";
		if (hour){
			cout << hour << "h ";
		};
		if (min){
			cout << min << "m ";
		};
		if (sec){
			cout << sec << "s ";
		};
		if (result_time){
			cout << result_time << "ms";
		};
		cout << "." << endl;
	} else {
		cout << ">> There is nothing to work with. Drag files into app or use console." << endl;
	}

	system("pause");
	return 0;
}