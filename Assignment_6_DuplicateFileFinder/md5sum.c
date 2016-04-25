// Neil Culbertson

#include "md5sum.h"



/*
	Calculates the md5sum of the filename and returns the character
	value of the md5sum at the specified index.

	@param filename, the file we're calculating the md5sum for
	@param index, the position in the md5sum char array
		This param is mainly for the method put_MD5sum_In_Array
		method so we can put the md5sum in a character array.

	@return char, the character at the given index in the md5sum
	array

	This md5sum implementation was found at:
	http://stackoverflow.com/questions/10324611/how-to-calculate-the-md5-hash-of-a-large-file-in-c
	and was modified a little bit to work with my program.
*/
char calculateMD5sum(char *filename, int index){
	unsigned char c[MD5_DIGEST_LENGTH];
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        // printf ("%s can't be opened.\n", filename);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
    MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    fclose (inFile);
    return c[index];
}

/*
	Puts the md5sum in a character array.
	
	@param md5sum, the array we're putting the md5sum in.
	@param filename, the file's name we're calculating the
		md5 sum for.
*/
void put_MD5sum_In_Array(unsigned char md5sum[], char *filename){
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
		md5sum[i] = calculateMD5sum(filename, i);
	}
}


/*
	Prints the md5sum and the file.
*/
void printMD5sum(unsigned char md5sum[], char *filename){
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", md5sum[i]);
	printf("  %s\n", filename);
}


/*
	Puts the md5sum in a character array.
	
	@param filename, the file's name we're calculating the
		md5 sum for.
*/
void put_MD5sum_In_struct(char *fname){
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
		hashes->hash[i] = calculateMD5sum(fname, i);
	}
}

void fillHashArray(unsigned char md5sum[]){
	 for(int i = 0; i < 16; i += 1){
    	hashes->hash[i] = md5sum[i];
    }
}