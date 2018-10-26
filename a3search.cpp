#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "porter2_stemmer.h"
#include <map>
#include <algorithm>
#include <sys/stat.h>
#include <unordered_map>
#include <sstream> 

using namespace std;


vector<string> stopwords {"able", "about", "above", "according", "accordingly", "across", "actually", "after", "afterwards", "again", "against", "all", "allow", "allows", "almost", "alone", "along", "already", "also", "although", "always", "among", "amongst", "and", "another", "any", "anybody", "anyhow", "anyone", "anything", "anyway", "anyways", "anywhere", "apart", "appear", "appreciate", "appropriate", "are", "around", "aside", "ask", "asking", "associated", "available", "away", "awfully", "became", "because", "become", "becomes", "becoming", "been", "before", "beforehand", "behind", "being", "believe", "below", "beside", "besides", "best", "better", "between", "beyond", "both", "brief", "but", "came", "can", "cannot", "cant", "cause", "causes", "certain", "certainly", "changes", "clearly", "com", "come", "comes", "concerning", "consequently", "consider", "considering", "contain", "containing", "contains", "corresponding", "could", "course", "currently", "definitely", "described", "despite", "did", "different", "does", "doing", "done", "down", "downwards", "during", "each", "edu", "eight", "either", "else", "elsewhere", "enough", "entirely", "especially", "etc", "even", "ever", "every", "everybody", "everyone", "everything", "everywhere", "exactly", "example", "except", "far", "few", "fifth", "first", "five", "followed", "following", "follows", "for", "former", "formerly", "forth", "four", "from", "further", "furthermore", "get", "gets", "getting", "given", "gives", "goes", "going", "gone", "got", "gotten", "greetings", "had", "happens", "hardly", "has", "have", "having", "hello", "help", "hence", "her", "here", "hereafter", "hereby", "herein", "hereupon", "hers", "herself", "him", "himself", "his", "hither", "hopefully", "how", "howbeit", "however", "ignored", "immediate", "inasmuch", "inc", "indeed", "indicate", "indicated", "indicates", "inner", "insofar", "instead", "into", "inward", "its", "itself", "just", "keep", "keeps", "kept", "know", "knows", "known", "last", "lately", "later", "latter", "latterly", "least", "less", "lest", "let", "like", "liked", "likely", "little", "look", "looking", "looks", "ltd", "mainly", "many", "may", "maybe", "mean", "meanwhile", "merely", "might", "more", "moreover", "most", "mostly", "much", "must", "myself", "name", "namely", "near", "nearly", "necessary", "need", "needs", "neither", "never", "nevertheless", "new", "next", "nine", "nobody", "non", "none", "noone", "nor", "normally", "not", "nothing", "novel", "now", "nowhere", "obviously", "off", "often", "okay", "old", "once", "one", "ones", "only", "onto", "other", "others", "otherwise", "ought", "our", "ours", "ourselves", "out", "outside", "over", "overall", "own", "particular", "particularly", "per", "perhaps", "placed", "please", "plus", "possible", "presumably", "probably", "provides", "que", "quite", "rather", "really", "reasonably", "regarding", "regardless", "regards", "relatively", "respectively", "right", "said", "same", "saw", "say", "saying", "says", "second", "secondly", "see", "seeing", "seem", "seemed", "seeming", "seems", "seen", "self", "selves", "sensible", "sent", "serious", "seriously", "seven", "several", "shall", "she", "should", "since", "six", "some", "somebody", "somehow", "someone", "something", "sometime", "sometimes", "somewhat", "somewhere", "soon", "sorry", "specified", "specify", "specifying", "still", "sub", "such", "sup", "sure", "take", "taken", "tell", "tends", "than", "thank", "thanks", "thanx", "that", "thats", "the", "their", "theirs", "them", "themselves", "then", "thence", "there", "thereafter", "thereby", "therefore", "therein", "theres", "thereupon", "these", "they", "think", "third", "this", "thorough", "thoroughly", "those", "though", "three", "through", "throughout", "thru", "thus", "together", "too", "took", "toward", "towards", "tried", "tries", "truly", "try", "trying", "twice", "two", "under", "unfortunately", "unless", "unlikely", "until", "unto", "upon", "use", "used", "useful", "uses", "using", "usually", "uucp", "value", "various", "very", "via", "viz", "want", "wants", "was", "way", "welcome", "well", "went", "were", "what", "whatever", "when", "whence", "whenever", "where", "whereafter", "whereas", "whereby", "wherein", "whereupon", "wherever", "whether", "which", "while", "whither", "who", "whoever", "whole", "whom", "whose", "why", "will", "willing", "wish", "with", "within", "without", "wonder", "would", "would", "yes", "yet", "you", "your", "yours", "yourself", "yourselves", "zero"};
unordered_map<string, char> stopwordMap;
unsigned char c;
string singleWord = "";
int MAXBUFFER = 2000000;
int currentSize = 0;
unsigned short tmpId = 0;
unsigned short fileId = 1;
vector<string> terms;

typedef pair<unsigned short, unsigned short> fileid_freq;
map<string, vector<fileid_freq>> totalFreq;
unordered_map<string, unsigned short> singleFileFreq;
map<unsigned short, string> fileNameGroup;
map<unsigned short, unsigned short> searchTermIndex;
vector< map<unsigned short, unsigned short> > totalSearchTermIndex;


bool indexFileExists(char* indexFolder);
bool cmpTerms(const string &p1, const string &p2);
bool cmp(const pair<string, float>  &p1, const pair<string, float> &p2);

void readFileName(string fileNamePath);
void wirteIndex(string indexFolder, unsigned short tmpId);
void mergeOneToAll(unordered_map<string, unsigned short> singleFileFreq);
void writeFileName(ofstream& fileNameOutput, string indexFolder, string fileName, unsigned short indexFileName);
void printResult(map<unsigned short, unsigned short> result);
void mergeTmpFiles(string indexFolder);

int searchTermIndexBuild(string indexFolder);
unsigned short getNumberOfTmp(string indexFolder);

string tokenisationKey(string searchTerm);
string mergeTmp(string tmp_0, string tmp_1, int tmpNum, string indexFolder);

map<unsigned short, unsigned short> joinPostings();
map<unsigned short, unsigned short> intersect(map<unsigned short, unsigned short> m, map<unsigned short, unsigned short> n);




int main(int argc, char **argv){
	string folder = argv[1];
	string indexFolder = argv[2];
	ofstream fileNameMap;
	int termStart = 3;
	// float cnum = 0.0;

	if(string(argv[3]).compare("-c") == 0){
		// cnum = stof(string(argv[4]));
		termStart = 5;
	}


	// put all the searching terms into a vector and sort them ascendingly
    for(int i = termStart; i <= argc - 1; i++){
    	terms.push_back(tokenisationKey(argv[i]));
    }
    
	sort(terms.begin(), terms.end(), cmpTerms);




	// index file not exist
	if(!indexFileExists(argv[2])){
		// stopwords
		for(unsigned int i = 0; i < stopwords.size(); i++){
			stopwordMap[stopwords[i]] = '1';
		}
		// create index folder
		system(("mkdir -p " + indexFolder).c_str());

		// open text folder
		DIR *dir = NULL;
		struct dirent *ent = NULL;
		dir = opendir(argv[1]);

		if(dir != NULL){
			// get each file in this directory
			while((ent = readdir(dir)) != NULL){
				if(string(ent->d_name).compare(".") == 0 || string(ent->d_name).compare("..") == 0 || string(ent->d_name).compare(".DS_Store") == 0){
					continue;
				}else{

					// write all the file name
					writeFileName(fileNameMap, indexFolder, ent->d_name, fileId);

					string filePath = folder + "/" + ent->d_name;
					
					ifstream fileInput(filePath, ios::in);

					while(fileInput.peek() != EOF){
						c = fileInput.get();
						if(isalpha(c)){
							singleWord.push_back(c);
						}else{
							if(singleWord.length() > 2){
								// word to lowercase
								transform(singleWord.begin(), singleWord.end(), singleWord.begin(), ::tolower);
								// check stopword
								if(stopwordMap[singleWord] != '1'){
									// stemming
									Porter2Stemmer::stem(singleWord);
									singleFileFreq[singleWord]++;
									currentSize++;				
								}
						
							}
							singleWord.clear();
						}
					}

					// save single file frequency to the total one
					mergeOneToAll(singleFileFreq);
	
					singleFileFreq.clear();

					// when MAXBUFFER reached, write out
					if(currentSize >= MAXBUFFER){
						wirteIndex(indexFolder, tmpId);
						totalFreq.clear();
						tmpId++;
						currentSize = 0;
					}

					// close each file
					fileInput.close();
					fileId++;
					
				}
			}
			// close DIR
			closedir(dir);
		}

		if(currentSize > 0){
			wirteIndex(indexFolder, tmpId);
			totalFreq.clear();
		}
		stopwordMap.clear();

		// merge all the tmp files into one
		mergeTmpFiles(indexFolder);

	}else{
		// comstruction filename map
		readFileName(indexFolder + "/" + "_filename");
	}

	// read posting list for the search terms
	int allPostingExist = searchTermIndexBuild(indexFolder);

	if(allPostingExist){
		// calculate the joined result
		map<unsigned short, unsigned short> result = joinPostings();
		if(result.size() > 0){
			// // print result
			printResult(result);
		}else{
			cout << endl;
		}

		
	}else{
		cout << endl;
	}
	


	return 0;
}



// check whether index folder exists
bool indexFileExists(char* indexFolder){
	struct stat sb;
	if(stat(indexFolder, &sb) == 0){
		return true;
	}else{
		return false;
	}

	return false;
}

// comparator for each terms, ordering them in ascending order
bool cmpTerms(const string &p1, const string &p2){	
	return p1 < p2;
}

// comparator for filename and frequency
// sort frequency descendingly
// sort the filenames in lexicographical order if frequencies are same
bool cmp(const pair<string, float>  &p1, const pair<string, float> &p2){	
	if(p1.second != p2.second){
		return p1.second > p2.second;
	}else{
		return p1.first < p2.first;
	}
}

// stemming each searching term and tramsform to lowercase
string tokenisationKey(string searchTerm){
	transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);
	Porter2Stemmer::stem(searchTerm);
	

	return searchTerm;
}

// write filenames to file
void writeFileName(ofstream& fileNameOutput, string indexFolder, string fileName, unsigned short indexFileName){
	fileNameGroup[indexFileName] = fileName;
	fileNameOutput.open(indexFolder + "/" + "_filename", ios::out | ios::app);
	fileNameOutput << fileName << endl;
	fileNameOutput.close();
}

// put single file frequency map to the total one
void mergeOneToAll(unordered_map<string, unsigned short> singleFileFreq){
	for(unordered_map<string, unsigned short>::iterator it = singleFileFreq.begin(); it != singleFileFreq.end(); it++){
		totalFreq[it->first].push_back(make_pair(fileId, it->second));
	}
}

// write temporary index file
void wirteIndex(string indexFolder, unsigned short tmpId){
	ofstream outputIndex(indexFolder + "/tmp_" + to_string(tmpId), ios::out | ios::app);
	for(map<string, vector<fileid_freq>>::iterator it = totalFreq.begin(); it != totalFreq.end(); it++){
		outputIndex << it->first << ":";
		for(vector<fileid_freq>::iterator postingIt = it->second.begin(); postingIt != it->second.end(); postingIt++){
			outputIndex << (*postingIt).first << "," << (*postingIt).second << ";";
		}

		outputIndex << endl;
	}

	outputIndex.close();
}

// read filenames and build a map for them
void readFileName(string fileNamePath){
	ifstream fileGroup(fileNamePath, ios::in);
	string filename;
	int i = 1;
	while(fileGroup.peek() != EOF){
		getline(fileGroup, filename);
		fileNameGroup[i] = filename;
		i++;
	}
	fileGroup.close();
}


// get the number of temporary index files
unsigned short getNumberOfTmp(string indexFolder){
	DIR *dirtmp = opendir(indexFolder.c_str());
    struct dirent *entmp = NULL;
    unsigned short count = 0;

    if(dirtmp != NULL){
        while((entmp  = readdir(dirtmp)) != NULL){
            if(string(entmp->d_name).compare(".") == 0 || string(entmp->d_name).compare("..") == 0 || string(entmp->d_name).compare(".DS_Store") == 0 || string(entmp->d_name).compare("_filename") == 0){
                continue;
            }else{
                count++;
            }

        }
    }
    closedir(dirtmp);

    return count;
}


// merge temporary index files
string mergeTmp(string tmp_0, string tmp_1, int tmpNum, string indexFolder){
    string mergeFile = "tmp_" + to_string(tmpNum);
    ofstream result(indexFolder + "/" + mergeFile, ios::out | ios::app);
    ifstream tmp0(indexFolder + "/" + tmp_0, ios::in);
    ifstream tmp1(indexFolder + "/" + tmp_1, ios::in);

    string line0;
    string line1;

    string first0;
    string first1;

    getline(tmp0, line0);
    getline(tmp1, line1);

    bool stop = false;
    while(!stop){
        first0 = line0.substr(0, line0.find(":", 0));
        first1 = line1.substr(0, line1.find(":", 0));

        if(first0.compare(first1) < 0){
            result << line0 << endl;
            getline(tmp0, line0);

        }else if(first0.compare(first1) == 0){
            result << line0;
            result << line1.substr(line1.find(":", 0) + 1, line1.length() - line1.find(":", 0) + 1) << endl;
            getline(tmp0, line0);
            getline(tmp1, line1);
        }else{
            result << line1 << endl;
            getline(tmp1, line1);

        }
        if(line0.length() == 0){
            if(line1.length() != 0){
                result << line1 << endl;
            }
            while(getline(tmp1, line1)){
                result << line1 << endl;
            }
            stop = true;
        }

        if(line1.length() == 0){
            if(line0.length() != 0){
                result << line0 << endl;
            }
            while(getline(tmp0, line0)){
                result << line0 << endl;
            }
            stop = true;
        }

        if(line0.length() == 0 && line1.length() == 0){
            stop = true;
        }
    }

    tmp0.close();
    tmp1.close();
    result.close();

    string path = indexFolder + "/" + tmp_0;
    remove(path.c_str());
    path = indexFolder + "/" + tmp_1;
    remove(path.c_str());

    return mergeFile;
}


// merge temporary index files
void mergeTmpFiles(string indexFolder){
	unsigned short numberOfTmp = getNumberOfTmp(indexFolder);
	unsigned short tmpNum = numberOfTmp;

	string mergeFile = "tmp_0";
	for(int i = 1; i < numberOfTmp; i++){
        mergeFile = mergeTmp(mergeFile, "tmp_" + to_string(i), tmpNum, indexFolder);
        tmpNum++;
    }
    tmpNum--;

    // change name of the merged index
    string oldName = indexFolder + "/tmp_" + to_string(tmpNum);
    string newName = indexFolder + "/index";
    rename(oldName.c_str(), newName.c_str());
}

// build maps for each searching terms
int searchTermIndexBuild(string indexFolder){
	ifstream readIndex(indexFolder + "/index", ios::in);
	int sizeOfTerms = terms.size();
    int i = 0;

	string line;
	string dic;
	stringstream posting;

	unsigned short fileId;
	unsigned short fre;

	char c;
	string s;

	while(getline(readIndex, line)){
		dic = line.substr(0, line.find(":", 0));
		if(dic == terms[i]){
			posting << line.substr(line.find(":", 0) + 1, line.length() - line.find(":", 0) + 1);

			while(posting.peek() != EOF){
				c = posting.get();
				if(c == ','){
					fileId = stoi(s);
					s.clear();
				}else if(c == ';'){
					fre = stoi(s);
					searchTermIndex[fileId] = fre;
					s.clear();
				}else{
					s.push_back(c);
				}
				
			}
			posting.clear();

			totalSearchTermIndex.push_back(searchTermIndex);
			searchTermIndex.clear();
			i++;
		}

		if(i >= sizeOfTerms){
			break;
		}
	}
	readIndex.close();

	if(totalSearchTermIndex.size() == terms.size()){
		return 1;
	}else{
		return 0;
	}

}


// find intersection of all the posting lists
map<unsigned short, unsigned short> intersect(map<unsigned short, unsigned short> m, map<unsigned short, unsigned short> n){
	map<unsigned short, unsigned short> result;
	map<unsigned short, unsigned short> ::iterator it_m = m.begin();
	map<unsigned short, unsigned short> ::iterator it_n = n.begin();

	while(it_m != m.end() && it_n != n.end()){
        if(it_m->first == it_n->first){
            result[it_m->first] = it_m->second + it_n->second;
            it_m++;
            it_n++;
        }else if(it_m->first < it_n->first){
            it_m++;
        }else{
            it_n++;
        }
    }
    return result;
}


// find intersection of all the posting lists
map<unsigned short, unsigned short> joinPostings(){
	map<unsigned short, unsigned short> resultInt = totalSearchTermIndex[0];

	// intersect each posting lists
	for(unsigned short i = 1; i < totalSearchTermIndex.size(); i++){
		resultInt = intersect(totalSearchTermIndex[i], resultInt);
	}

	return resultInt;
}


// pring the final result
void printResult(map<unsigned short, unsigned short> result){
	int nums = terms.size();

	map<string, float> newResult;
	for(map<unsigned short, unsigned short>::iterator it = result.begin(); it != result.end(); it++){
		newResult[fileNameGroup[it->first]] = (float)it->second / (float)nums;
	}

	// sort the result
	vector<pair<string, float> > v;
	copy(newResult.begin(), newResult.end(), back_inserter(v));
    sort(v.begin(), v.end(), cmp);
    for(unsigned short i = 0; i < v.size(); ++i){
    	cout << v[i].first << endl;
    }

}
