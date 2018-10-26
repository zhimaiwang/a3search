------
Usage:
------
./a3search <destination_folder> <index_folder> term1 term2 ...

example:
./a3search testfiles/books200m books200m.idx "apple" "banana" "cat"

----------
Index File
----------
format:
token:     fileId, frequency; fileId, frequency;
~~~~~~     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dictionary            posting list

example:
appl:1,1;3,2;5,1;6,3;




------------------
Design Description
------------------
1. Toenization and Stemming
a. open the destination folder and read each original files
b. read the files character by character and check whether the single letter is alphabet or not. If so, append it to the previous string; if not, a single word has been constructed
c. check word length and ignore it if its length is less than 3
d. use Porter Stemming Algorithm (Porter2Stemmer library) to remove morphological endings
e. fitler stopwords


2. Frequncy Counting
a. an unordered map is builded for each file
b. put each useful word in the map above mentioned and add by one
c. when one file is finished, put it to the total frequency map
d. clear the single file map to save memory


3. Writing Temporary Index Files
a. set a buffer index, MAXBUFFER = 2000000
b. keep a counter, adding by one when the single file map is updated
c. when the counter is greater than MAXBUFFER, output all the frequency to temporary file
d. clear the total frequency map and initialise the counter to 0


4. Merging Temporaty Index Files
a. merge two index files everytime, say file1 and file2
b. read line by line and get the dictionary
c. if the two dictionarys are same, group the postings of both files and write to the merged file
d. if they are different, insert the small one to the merged file and leave the larger one to be compared again
e. if one file is finished read, append the rest of the other file to the merged file
f. after all the index files have been merged, rename the final merged file as "index"


5. Writing/Reading File Names
a. assign each file name with an index to save memory
b. write all the file name to the index folder at the first time, names "_filename"
c. read "_filename" to construct the filename map for the subsequent searching


6. Searching
a. read index file line by line and get the dictionary
b. if the dictionary is same as the term, get its posting list, build map for this term and put it to the vector of total search term index, and then move to the next searching term (searching terms are sorted in ascending order)


7. Joining
a. scan two posting lists from the begining
b. if the file id occurs in both postings, save it in a map, and move pointers of both posting list forward
c. if not same, move the pointer with small file id forward
d. intersect until one of them reaches the tail


8. Ranking
a. calculate the average frequency of the final intersected result
b. rank the average frequency descendingly
c. if the average frequencies are same, sort the file name in the lexicographical order

