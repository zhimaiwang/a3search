#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "porter2_stemmer.h"
#include <map>

using namespace std;
map<string, int> counts;
const char * delimiter = " \\s[*$&#\\/\"'\\,.:;?!\\[\\](){}<>~\\-_]+";



vector<string> splitLine(string& line, string& delimiter){ // whether add 3 const
	string buff {""};
	vector<string> result;

	for(auto c: line){
		if(delimiter.find(c) == string::npos){
			buff += c;
		}else if(delimiter.find(c) != string::npos && buff != ""){
			if(buff.length() > 2){
				result.push_back(buff);
			}
			buff = "";
		}
	}

	if(buff != ""){
		result.push_back(buff);
	}

	return result;
}


void tokenisation(char *token){
	while(token){
		if(strlen(token) > 2){
			cout << token << "    --->    ";
			string tokenStemed(token);
			Porter2Stemmer::stem(tokenStemed);
			cout << tokenStemed << endl;

			if(counts.find(tokenStemed) == counts.end()){
				counts[tokenStemed] = 1;
			}else{
				counts[tokenStemed]++;
			}

		}
		token = strtok(NULL, delimiter);
	}
}


void writeTermIndex(ofstream& termIndex, string indexFolder, int indexFileName){
	for(map<string, int>::iterator it = counts.begin(); it != counts.end(); it++){
		// cout << it->first << " => " << it->second << endl;
		termIndex.open(indexFolder + "/" + it->first, ios::out | ios::app);
		termIndex << indexFileName << ":" << it-> second << endl;
		termIndex.close();
	}
}

int main(int argc, char **argv){

	string folder = argv[1];
	DIR *dir = NULL;
	struct dirent *ent = NULL;

	

	int indexFileName = 1;

	// create index folder
	string indexFolder(argv[2]);
    system(("mkdir -p " + indexFolder).c_str());
    ofstream termIndex;

	// open directory
	dir = opendir(argv[1]);
	if(dir != NULL){
		// get each file in this directory
		while((ent = readdir(dir)) != NULL){
			if(string(ent->d_name).compare(".") == 0 || string(ent->d_name).compare("..") == 0 || string(ent->d_name).compare(".DS_Store") == 0){
				continue;
			}else{
				// cout << ent->d_name << endl;
				string filePath = folder + "/" + ent->d_name;
				
				// open file
				ifstream fileInput(filePath, ios::in);
				string line;
				
				while(getline(fileInput, line)){
					transform(line.begin(), line.end(), line.begin(), ::tolower);
					// cout << line << endl;
					// cout << "split:" << endl;
					// vector<string> lineSplit = splitLine(line, delimiter);
					char *lineToCharArray = strdup(line.c_str());
					char *token = strtok(lineToCharArray, delimiter);
					
					tokenisation(token);
					
				}

				writeTermIndex(termIndex, indexFolder, indexFileName);
				

				cout << "----" << endl;
				// indexFileName++;
				counts.clear();
				fileInput.close();
				
				indexFileName++;
			}
		}
		closedir(dir);
	}else{
		cout << "cannot open the dir" << endl;
	}

	return 0;
}