/*
	Neil Culbertson
	Systems Programming
	Assignment 5: Duplicate Fine Finder
	Dr. Scott Kuhl
*/

#include "md5sum.h"

DIR *dp;
struct dirent *entry;
unsigned char c[MD5_DIGEST_LENGTH];
int numOfHashes = 0;
int lenOfCurDirectory;
char cwd[1024];
char directory[1024];
int numDir = 1;
int initial = 1;
/*
	Is the path a directory.

	@return 0 if it is a directory.
*/
int isDirectory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

/*
	Get's the directory
*/
void getDirectory(char newDir[]){
	for(int i = 0; i < 1024; i += 1){
		directory[i] = '\0';
	}
	int index = 0;
	for(int i = lenOfCurDirectory+1; i < (int)strlen(newDir); i += 1){
		directory[index] = newDir[i];
		index += 1;
	}
	if(strlen(directory) != 0) strcat(directory, "/");
}

/*
	Recursively finds all the hashes and fills them in the hashes struct.
*/
void getAllMD5Hash(DIR *dp, char *dirName){
	if( !(dp = opendir(dirName)) ){
		perror("opendir");
		return;
	}
	if( chdir(dirName) == -1){
		perror("chdir");
		return;
	}
	if(initial){
		getcwd(cwd, sizeof(cwd));
		lenOfCurDirectory = strlen(cwd);
		initial = 0;
	}

	readdir(dp); // ignore "."
	readdir(dp); // ignore ".."
	
	
	// fill information
	while( (entry = readdir(dp)) != NULL){
		if(isDirectory(entry->d_name)){
			numDir += 1; // we are in another level of directories
			getAllMD5Hash(dp, entry->d_name);
		}else{
			put_MD5sum_In_Array(c, entry->d_name);
			memcpy(hashes[numOfHashes].hash, c, 16);
			// get directory
			getcwd(cwd, sizeof(cwd));
			getDirectory(cwd);
			// put directory in filename
			strcat(directory, entry->d_name);
			hashes[numOfHashes].filename = malloc(sizeof(directory));
			// put filename & directory in hash
			strcpy(hashes[numOfHashes].filename, directory);
			numOfHashes += 1;
		}
	}
	if(numDir > 1){
		chdir(".."); // go back directory to read remaining files.
		numDir -= 1;
	}
}

/*
	Compares the hash

	@return 0 is they're equal.
*/
int compareHash(unsigned char a[], unsigned char b[]){
	for(int i = 0; i < 16; i += 1){
		if(a[i] != b[i]) return 1;
	}
	return 0;
}

/*
	Finds the duplicate hashes and prints them out.
*/
void findDuplicate(){
	for(int i = 0; i < numOfHashes; i += 1){
		for(int j = i+1; j < numOfHashes; j += 1){
			if( (compareHash(hashes[i].hash, hashes[j].hash)) == 0 ){ 
				if(hashes[i].dir) printf("%s%s ", hashes[i].dir, hashes[i].filename);
				else printf("%s ", hashes[i].filename);
				if(hashes[j].dir) printf("%s%s\n", hashes[j].dir, hashes[j].filename);
				else printf("%s\n", hashes[j].filename);
			}
		}
		free(hashes[i].filename);
	}
}

int main(int argc, char *argv[]){
	(void)argc;
	// hashes = malloc(sizeof(hashes)*300);
	// find all hashes
	getAllMD5Hash(dp, argv[1]);
	// find the duplicates
	findDuplicate();
	// free(hashes);
    return 0; 
}