/*
	Neil Culbertson
	CS3411 Systems Programming
	January 9th, 2016
	Assignment 2

	Tar-like program; Appends, extracts, and deletes files from the archive.


	WHAT NEEDS FIXING:
		- Extract example.mtu with valgrind
			* Lost 5 points because: extracted script.sh has wrong permissions.
		- Run without an argument after -x (with valgrind)
			* Lost 1 points because: valgrind.txt: Valgrind error count: 1
		- Try extracting an invalid archive (invalid.mtu is an empty file) (with valgrind)
			* Lost 6 points because: valgrind.txt: Valgrind error count: 6
		- Check that basename() is used.
			* Lost 5 points because: Exit status: Program exited due to a signal (segfault?)
			* Lost 2 points because: md5sum: textfile.txt should have hash b264e4c43787cb81e7d4c5eb4b006427 but it is MISSING.
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	char file_name[256]; /* May contain spaces, is null-terminated --- must be the basename() of the file (man 3 basename) */ 
	uint64_t file_size;  /* File size in bytes. 8 bytes */ 
	uint8_t deleted;     /* 0=file, 1=deleted file, 1 byte */ 
	mode_t mode;         /* See "man 2 stat", 4 bytes */ 
} mtuhdr;

mtuhdr info;
char magicBytes [] = {'C', 'S', '3', '4', '1', '1'};

/*
	Reads the first 6 bytes of the file to make sure
	it is equal to CS3411. If it isn't, returns false.

	@param file, the file we're reading from.
	@return bool, true if magic bytes equal CS3411, false
		otherwise.
*/
bool mtuBytes(FILE *file){
	bool rightBytes = true;
	unsigned char c;

	for(int i = 0; i < 6; i += 1){
		if(fread(&c, 1, 1, file) != 1){
			perror("Error readng file.");
		}
		if(c != magicBytes[i]){
			rightBytes = false;
		}
	}
	return rightBytes;
}

/*
	Get input from user to determine if the file wants
	to be overwritten. 'y' or 'Y' to overwrite the file, 
	otherwise file will not be overwritten.
*/
bool overWrite(){
	char c;
	scanf("%c", &c); // get the yes or no input
	getchar(); // consume the newline character
	if(c == 'y' || c == 'Y') return true;
	return false;
}

/*
	Prints help information for the user.
*/
void printHelpInfo(){
	printf("mtutar -a archive.mtu file1 file2 ... - Appends one or more files into a file named archive.mtu.\n\n");
	printf("mtutar -d archive.mtu file1 - Deletes a single file (file1) from the archive.\n\n");
	printf("mtutar -x archive.mtu - Extracts the files from archive.mtu into the current directory.\n\n");
}

void printCharArray(char ch[256]){
	int index = 0;
	printf("Char: ");
	do{
		printf("%c", ch[index]);
		index += 1;

	}while(ch[index] != '\0');
	printf("\n");
}

/*
	Gets the files information including the files:
		- Name
		- Size
		- Deleted byte
		- Mode

	@param file, the file we're reading from
*/
void getFileInfo(FILE *file){
	// get file name
	char c;
	for(int i = 0; i < 256; i += 1){				// 256 bytes
		c = fgetc(file);
		info.file_name[i] = c;
	}

	// get files size
	if(fread(&info.file_size, 8, 1, file) != 1){	// 8 bytes
		perror("Error reading file");
		return;
	}				

	// get files deleted byte
	if(fread(&info.deleted, 1, 1, file) != 1){      // 1 byte
		perror("Error reading file");
		return;
	}
	
	// get files mode
	if(fread(&info.mode, 4, 1, file) != 1){			// 4 bytes
		perror("Error reading file");
		return;
	}
}

/*
	Appends all the information given by the current file to the archive
	file we're appending to.
*/
void appendFile(FILE *file, FILE *inputFile){
	char ch;
	fwrite(&info.file_name, 256, 1, file); // write file name
	fwrite(&info.file_size, 8, 1, file); // write file size 
	fwrite(&info.deleted, 1, 1, file); // write deleted byte
	fwrite(&info.mode, 4, 1, file); // write mode
	for(int i = 0; i < (int)info.file_size; i += 1){ // write contents to file
		if(fread(&ch, 1, 1, inputFile) != 1){
			perror("fread");
		}
		fwrite(&ch, 1, 1, file);
	}
}

/*
	Fills the file name into a sized 256 character array.

	@param file, the file's name we want to put in array.
	@array, the array we're filling.
*/
void fill256CharArray(char *file, char array[256]){
	int index = 0;
	do{
		array[index] = file[index];
		index += 1;
	}while(file[index] != '\0');
}
/*
	Append files together into one archive file type of mtu.

	@param argc, number of arguments from command line.
	@param argv, each argument from command line.
*/
int append(int argc, char *argv[]){
	struct stat fileStat;
	char *fileName = NULL;
	int fileExists = access(argv[2], F_OK);
	FILE *file = fopen(argv[2], "a");
	if(!file){
		perror(argv[2]);
		return 1;
	}
	if(!(fileExists == 0)){	// create new file with MTU bytes in header
		fwrite("CS3411", 1, 6, file);
	}

	// append each file
	for(int i = 3; i < argc; i += 1){
		
		fileName = basename(argv[i]);
		FILE *currFile = fopen(fileName, "r");
		
		if(!currFile){
			printf("this is where the error occurs basename: %s\n", fileName);
			perror(fileName);
			return 0;
		}
		
		char array[256];
		memset(&array[0], 0, sizeof(array)); // erase contents of array

		fill256CharArray(fileName, array); // need to do this to compare strings

		strncpy(info.file_name, array, 256);
		stat(fileName, &fileStat);
		info.file_size = fileStat.st_size;
		info.mode = fileStat.st_mode;

		appendFile(file, currFile); // fill contents to the archive file
		fclose(currFile); // close file
	}
	fclose(file);
	return 0;
}

/*
	Deletes file from archive. If archive file doesn't exist, it will print
	a message and return. If the file trying to be deleted doesn't exist in
	the archive file, nothing will happen and will print a message.
*/
int delete(char *argv[]){
	bool fileNotFound = true;
	char array[256];
	char *fileName = argv[3];

	int fileExists = access(argv[2], F_OK);
	if(fileExists != 0){
		printf("Archive file %s does not exist.\n", argv[2]);
		return 0;
	}

	FILE *file = fopen(argv[2], "r+");
	if(!file){
		perror(argv[2]);
		return 0;
	}
	fseek(file, 0, SEEK_END);
	int fileSize = (int)ftell(file);
	fseek(file, 0, SEEK_SET);
	mtuBytes(file); // move 6 bytes ahead

	fill256CharArray(fileName, array);

	int headerSize = 269;
	int position = 6;
	position += headerSize; // 

	do{
		getFileInfo(file);
		fseek(file, position, SEEK_SET); 
		position += (headerSize + (int)info.file_size);
		fseek(file, (ftell(file) + (int)info.file_size), SEEK_SET); // move to the next file in the archive file

		// check to see if we found the file name
		if(strcmp(array, info.file_name) == 0){
			int currentDistance = ftell(file); // set current position we're at in the archive file
			int distanceFromDeleted = (ftell(file)) - (info.file_size + 5); // set distance from deleted byte
			fileNotFound = false;
			fseek(file, distanceFromDeleted, SEEK_SET);
			fseek(file, 0L, SEEK_CUR); // switch from reading to writing
			fputc(1, file); // set deleted byte to 1
			fseek(file, 0L, SEEK_CUR); // switch from reading to writing
			fseek(file, currentDistance, SEEK_SET); // move back our current position in archive file
		}

		if(ftell(file) > fileSize){
			break;
		}

	}while(ftell(file) != fileSize);

	if(fileNotFound) printf("File not found in archive file.\n");
	fclose(file);

	return 0;
}


/*
	Fills the extracted file with the necessary contents.

	@param newFile, the file we're filling.
	@param oldFile, the file that contains all the necessary
		information inside the archive file. 
*/
int fillFile(FILE *newFile, FILE *oldFile){
	char c;
	if(info.deleted == 1){ // if files deleted then move to next file and return
		fseek(oldFile, (ftell(oldFile) + (int)info.file_size), SEEK_SET);
		return 0;
	}
	if(newFile){ // if new file already exists, ask user if they want to over write
		printf("File '%s' exists Overwrite? (Y, y)", info.file_name);
		if(overWrite()){
			newFile = fopen(info.file_name, "w");
			if(!newFile){
				perror("Error");
				return 1;
			}
			for(int i = 0; i < (int)info.file_size; i += 1){ // overwrite file
				fread(&c, 1, 1, oldFile);
		      	fputc(c, newFile);
		 	}
		}else {
			fseek(oldFile, (ftell(oldFile) + (int)info.file_size), SEEK_SET); // move the old file to the right position
		}
	}
	else{ // file doesnt exist yet, make a new one
		newFile = fopen(info.file_name, "w");
		if(!newFile){
			perror("Error");
			return 1;
		}
		for(int i = 0; i < (int)info.file_size; i += 1){
			fread(&c, 1, 1, oldFile);
	      	fputc(c, newFile);
	 	}
	}
	fclose(newFile);
	return 0;
}

/*
	Extracts the files from the mtu archive file.
*/
int extract(char *argv[]){
	FILE *file;
	FILE *tempFile;

	file = fopen(argv[2], "r"); // open archive file
	if(!file){
		perror("error opening file");
		return 1;
	}

	// calculate the size of the file
	fseek(file, 0, SEEK_END);
	int fileSize = (int)ftell(file);
	fseek(file, 0, SEEK_SET);
	if(fileSize == 7){
		printf("Archive file has no files\n");
		return 0;
	}
	// move 6 bytes into the archive file
	if(!mtuBytes(file)){							// 6 bytes
		printf("ERROR: Not right file type!\n");
		return 1;
	}

	int headerSize = 269;
	int position = 6;
	position += headerSize;
	
	// Extract each file
	do{
		getFileInfo(file); 

		tempFile = fopen(info.file_name, "r");
		chmod(info.file_name, info.mode); // set permissions 
		fseek(file, position, SEEK_SET); // move to the next file
		if(fillFile(tempFile, file) != 0){
			return 1;
		}

		position += (headerSize + (int)info.file_size);

		if(ftell(file) > fileSize){
			fclose(tempFile);
			break;
		}

	}while(ftell(file) != fileSize);

	fclose(file);

	return 0;
}

int main(int argc, char *argv[]){
	int returnCode = 0;
	if(argc == 1){
		printHelpInfo();
	}else{
		if(strcmp(argv[1], "-x") == 0){ 
			returnCode = extract(argv);
		}
		else if(strcmp(argv[1], "-a") == 0){
			returnCode = append(argc, argv);
		}
		else if(strcmp(argv[1], "-d") == 0){
			returnCode = delete(argv);
		}
		else{ 
			printHelpInfo(); 
			return 0;
		}
	}
	if(returnCode == 0){
		printf("Returning 0!\n");
		return 0;
	} 
	printf("returning 1!\n");
	return 1;
}



