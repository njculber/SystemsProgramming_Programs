/*
	Neil Culbertson
	Spring 2016
	Systems Programming - Assignment 3 (Shared Libraries)
	Professor Scott Kuhl
*/
#define	_SVID_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "libmtu.h"

/*
	Counts the number of UTF-8 characters are in the string.

	@param *bytes, the string

	@return size, the number of UTF-8 characters.
*/
unsigned int mtu_countUTF8(char *bytes){
	unsigned char currChar = bytes[0];
	unsigned int size = 0;
	int count = 0;
	while(currChar != '\0'){
		if(currChar >> 7 == 0){
			count += 1;
		}else{
			if(currChar >> 5 == 6){
				currChar = bytes[size+2];
				count += 1;
			}
			else if(currChar >> 4 == 14){ 
				currChar = bytes[size+3];
				count += 1;
			}
			else if(currChar >> 3 == 30){ 
				currChar = bytes[size+4];
				count += 1;
			}
			else if(currChar >> 2 == 62){  // apparently these don't exist anymore
			 	currChar = bytes[size+5];
			 }
			else if(currChar >> 1 == 126){ // apparently these don't exist anymore
				currChar = bytes[size+6];
			}
		}
		size += 1;
		currChar = bytes[size];
	}
	return count;
}

/*
	Open's a file using the open method, writes string1 to the file, seeks
	to the position relevant to the beginning of the file, then writes the 
	second string.

	If file doesn't exist, it will create it, if it does exist it will overwrite
	it.

	@param filename, the filename
	@param string1, the first string
	@param seek, the positionwe're moving to in the file
	@param string2, the second string

	@return 1 on success, 0 otherwise.
*/
int mtu_writeFileOpen(char *filename, char *string1, int seek, char *string2){
	int file = open(filename, O_RDWR | O_CREAT| O_TRUNC, S_IRWXU);

	if(file == -1){
		perror("Error opening file");
		return 0;
	}
	if(write(file, string1, strlen(string1)) != (int)strlen(string1)){
		perror("Error writing to file");
		return 0;
	}
	if(seek >= 0 && seek < (int)strlen(string1)){
		if(lseek(file, seek, SEEK_SET) == -1){
			perror("Error seeking in file");
			return 0;
		}
	}
	if(write(file, string2, strlen(string2)) != (int)strlen(string2)){
		perror("Error writing to file");
		return 0;
	}
	if(close(file) == -1){
		perror("Error closing file");
		return 0;
	}

	return 1;
}

/*
	Open's a file using the fopen method, writes string1 to the file, seeks
	to the position relevant to the beginning of the file, then writes the 
	second string.

	If file doesn't exist, it will create it, if it does exist it will overwrite
	it.

	@param filename, the filename
	@param string1, the first string
	@param seek, the positionwe're moving to in the file
	@param string2, the second string

	@return 1 on success, 0 otherwise.
*/
int mtu_writeFileFopen(char *filename, char *string1, int seek, char *string2){
	FILE *file = fopen(filename, "w+");

	if(!file){
		perror("Error opening file");
		return 0;
	}
	if((int)fwrite(string1, 1, strlen(string1), file) != (int)strlen(string1)){
		perror("Error writing to file");
		fclose(file);
		return 0;
	}
	if(seek >= 0 && seek < (int)strlen(string1)){
		if(fseek(file, seek, SEEK_SET) == -1){
			perror("Error seeking in file");
			fclose(file);
			return 0;
		}
	}
	if((int)fwrite(string2, 1, strlen(string2), file) != (int)strlen(string2)){
		perror("Error writing to file");
		fclose(file);
		return 0;
	}
	if(fclose(file) == -1){
		perror("Error closing file");
		return 0;
	}

	return 1;
}

/*
	Returns the resident set size for the current process.

	@return int, the resident set size for the current process.
*/
unsigned int mtu_popen(){
	int pid = getpid();
	char command[200] = "";
	unsigned int neil; 
	sprintf(command, "ps -p %d -o rss=", pid);
	FILE *file = popen(command, "r");

	fscanf(file, "%u", &neil);

	return neil;
}

/*
	Returns 1 if negating the value will have defined result, 0 otherwise.

	@param, the number to check negation
	@return int, the success flag
*/
int mtu_canNegate(int a){
	if (a == INT_MIN){
		return 0;
	}
	return 1;
}

/*
	Returns 1 if dividing a by b will will return a defined result, 
	0 otherwise.

	@param a, the numerator
	@param b, the denominator

	@return int, the success flag.
*/
int mtu_canDivide(int a, int b){
	(void)a;
	if(b == 0){
		return 0;
	}
	if(a == INT_MIN && b == -1){
		return 0;
	}
	return 1;
}

/*
	This function is used for the sort method.

	@param a
	@param b
*/
int compare(const void * a, const void * b)
{
   return ( *(float*)a - *(float*)b );
}

/*
	Sorts the list in ascending order.

	@param list, the list we're sorting
	@param n, the size of the list
*/
void mtu_qsort(float *list, int n){
	qsort(list, n, sizeof(float), compare);
}

/*
	Return the current language your computer is using.
*/
char* mtu_lang(){
	return getenv("LANG");
}

/*
	Returns a list of pair sums given by the arguments.

	@param a, b, the first two paramters
	@param ..., the rest of the arguments.
*/
int* mtu_pairSum(int a, int b, ...){
	int numberOfPairs = 1; // aka the position we're at in the pair list
	int *pairs = malloc(sizeof(int)*numberOfPairs); // the list we're going to add the pair-sums to
	if(pairs == NULL){
		perror("Error Reallocing");
		return NULL;
	}
	int num1, num2;

	// if first two paramters are zero then set first element to zero in pointer and return
	if (a == 0 && b == 0){
		pairs[numberOfPairs-1] = 0;
		return pairs;
	}
	else{
		if (((b > 0) && (a > (INT_MAX - b))) || ((b < 0) && (a < (INT_MIN - b)))) { // check for undefined bahavior
			printf("Adding %d and %d will result in overflow\n", a, b);
			free(pairs);
			return NULL;
		}
		pairs[numberOfPairs-1] = (a + b); // add the pair-sum to the list
	}
	bool hasNotFoundTwoZeros = true; // flag for end of paramters
	va_list valist; // for getting the arguments
	va_start(valist, b);
	
	// while the next two paramters aren't both 0
	do{
		// get the next pair
		num1 = va_arg(valist, int);
		num2 = va_arg(valist, int);

		// next two paramters equal to zero, set flag to false
		if(num1 == 0 && num2 == 0){
			hasNotFoundTwoZeros = false;
		}
		else{
			// check for undefined behavior
			if (((num2 > 0) && (num1 > (INT_MAX - num2))) || ((num2 < 0) && (num1 < (INT_MIN - num2)))) {
				printf("Adding %d and %d will result in overflow\n", num1, num2);
				va_end(valist);
				return NULL;
			}
			// incrementing this also keeps track of position we're @ in the list
			numberOfPairs += 1;

			// add space to list to add the new pair-sum
			int *newPair = realloc(pairs, sizeof(int) * numberOfPairs); 
			if(newPair == NULL){
				perror("Error Reallocing");
				free(newPair);
				return NULL;
			}
			//pairs = newPairs;
			pairs = newPair;
			pairs[numberOfPairs-1] = (num1 + num2);
		}
	} while(hasNotFoundTwoZeros);
	//free(pairs);
	return pairs;
}

/*
	For comparing the file sizes. If the result is negative, 
	the second file is greater than the first file.
*/
int compareFile(const struct dirent **a, const struct dirent **b)
{
	struct stat file1;
	struct stat file2;

	stat((*a)->d_name, &file1);
	stat((*b)->d_name, &file2);
	// get directory name
	// get file size

   	return (file1.st_size - file2.st_size);
}

/*
	Prints all the files and thier sizes in the current directory.
*/
int mtu_printFiles(){
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL){ // get the current directory while also checking for error
		perror("Error getting directory!\n");
		return -1;
	}
	struct dirent **list;
	struct stat buf; // for getting size of file

	// get the number of files, and have all files stored in list
	int files = scandir(".", &list, NULL, compareFile);

	if(files < 0){ // check for error
		perror("Error reading directory!");
		return -1;
	}

	// print the files names and sizes
	for (int i = 0; i < files; i++) {
        struct dirent *entry;
        entry = list[i]; // get current file or directory
        printf("%s", entry->d_name); // print file name
        char *name = entry->d_name;
        stat(name, &buf);
        printf("% d\n", (int)buf.st_size); // print file size
        free(entry);
    }

	return files;
}


















