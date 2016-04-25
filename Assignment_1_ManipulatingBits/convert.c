/* 
	Neil Culbertson
	CS3411 Systems Programming
	Spring 2016
	Assignment 1

	This program reads a file that contains a number of bits representing
	a character, float, or an int. It reads the bits, and outputs the value
	of those bits given whether it's a character, float, or an int.
*/
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>


unsigned char currentChar; // this is for keeping track of a byte in the file
int currentBit = 0;		   // this is for keeping track of the bit in the byte

/*
	Checks if the file is empty by moving to the end of the file, 
	and if the position is still zero, then this means there is no
	data.

	@return bool, false if the file is not empty, true otherwise.
*/
bool isEmpty(FILE *file){
    long offset = ftell(file);
    fseek(file, 0, SEEK_END); // move to the end of the file

    // if the ftell function returns 0, we are at the end of the file
    if (ftell(file) == 0){
        return true;
    }

    fseek(file, offset, SEEK_SET);
    return false;
}
/*
	Returns the next bit from the file.

	@param *file, the file we're reading from

	@return int, either a 0 or a 1
*/
int getNextBit(FILE *file){
	// if the currntbit is a 0, then we must read the next byte from file.
	if(currentBit == 0){
		fread(&currentChar, 1, 1, file); // read a byte
	}
	unsigned char temp = currentChar;
	/* 
		Bit shift all the way to the left, then all the way to the right 
		to get either 00000000 or 00000001
	*/
	temp = temp << currentBit;
	temp = temp >> 7;
	// increment bit
	currentBit += 1;
	// once we reach the end of byte, set current bit to 0
	if(currentBit == 8){
		currentBit = 0;
	}
	// return int value of the bit-shifted character
	return (int)temp;
}

/*
	Calculates the integer value.

	@param binaryArray, the array filled with 0's & 1's
	@param arraySize, the size of the binaryArray[]

	@return int, the value of the 0's and 1's
*/
int calculateInt(int binaryArray[], int arraySize){
	uint32_t result = 0;
	/*
		ORing all the ones and zeros into the character 'result'
		by looping through the binary array. 

		Once it locates a '1', it will bit shift the '1' left however
		many times we are at in the array and OR it with the result.
	*/
	for (int i = 0; i < arraySize; ++i){
    	result |= (binaryArray[i] == 1) << ((arraySize-1) - i);
    }
    return result;
    
}

/*
	Calculates the character value.
*/
char calculateChar(int binaryArray[], int arraySize){
	unsigned char result = 0x0000;
	/*
		ORing all the ones and zeros into the character 'result'
		by looping through the binary array. 

		Once it locates a '1', it will bit shift the '1' left however
		many times we are at in the array and OR it with the result.
	*/
	for (int i = 0; i < arraySize; ++i){
    	result |= (binaryArray[i] == 1) << ((arraySize-1) - i);
    }
    return result; 
}

/*
	Calculates the float value by utilizing union and uint32_t
*/
float calculateFloat(int binaryArray[], int arraySize){
	// initialize union with a type uint32_t and a float
	typedef union{
		uint32_t val;
		float f;
	} type;

	// declare instance of type and set to 0
	type u;
	u.val = 0;
	/*
		ORing all the ones and zeros into the int 'u.val'
		by looping through the binary array. 

		Once it locates a '1', it will bit shift the '1' left however
		many times we are at in the array and OR it with the result.
	*/
	for (int i = 0; i < arraySize; ++i){
    	u.val |= (binaryArray[i] == 1) << ((arraySize-1) - i);
    }
    // return the float version of type
    return u.f;
}

/*
	Calculates the value of the binary string.
*/
int value(int binaryArray[], int arraySize){
	int value = 0; // total value of bit string
	int pos = 0;   // # of times we've read through the array
	/*
		Reading left to right in the array, once we see a 
		'1' we raise 2 to the power of pos, where pos is
		how many elements we've read in the array. Add this 
		value to our current value
	*/
	for(int i = 1; i < arraySize+1; i += 1){
		if(binaryArray[arraySize-i] == 1){
			value += pow(2, pos);
		}
		pos += 1;
	}
	return value;
}

/*
	Calculates the integer value.

	@param file, the file we are reading from

	@return int, the integer value
*/
int itsAnInt(FILE *file){
	int numOfBits[5];
	int size = sizeof(numOfBits)/sizeof(int);
	for(int i = 0; i < 5; i += 1){
		numOfBits[i] = getNextBit(file);
	}
	int bitSize = (value(numOfBits, size)) + 1;
	int intValue[bitSize];
	for(int i = 0; i < bitSize; i += 1){
		intValue[i] = getNextBit(file);
	}
	int number = calculateInt(intValue, sizeof(intValue)/sizeof(int));

	return number;
}

/*
	Calculates the char value.

	@param file, the file we're reading from.

	@return char, the character value
*/
char itsAChar(FILE *file){
	int byteArray[7];
	int size = sizeof(byteArray)/sizeof(int);
	for(int i = 0; i <= 6; i += 1){
		byteArray[i] = getNextBit(file);
	}
	//getNextBit(file);
	return calculateChar(byteArray, size);
}

/*
	Calculates the float value.

	@param file, the file we're reading from

	@return float, the float value
*/
float itsAFloat(FILE *file){
	int floatArray[32];
	for(int i = 0; i < 32; i += 1){
		floatArray[i] = getNextBit(file);
	}
	return calculateFloat(floatArray, 32);;
}

/*
	Reads the next two bits to determine the next
	datatype in the file.
*/
int getDataType(FILE *file){
	// declare size 2 array
	int bits [2];
	bits[0] = getNextBit(file);
	bits[1] = getNextBit(file);

	/*
		Return 0 if it's an int, 1 if it's a float, 2 if it's
		a character, and 0 if end of file
	*/
	if(bits[0] == 0 && bits[1] == 0) 	  return 2;
	else if(bits[0] == 0 && bits[1] == 1) return 0;
	else if(bits[0] == 1 && bits[1] == 0) return 1;
	else if(bits[0] == 1 && bits[1] == 1) return 3;
	else{
		printf("ERROR: DID NOT RECOGNIZE DATATYPE");
		return -1;
	}
}

int main(void){
	// read the file as binary
	FILE *file = fopen("file.dat", "rb");
	// declare bool to indicate it's not end of file
	bool endOfFile = false;
	if(file == NULL) { 
		printf("File not successfully read.\n");
		return 1;
	}
	else if(isEmpty(file)){
		return 1;
	}
	else{
		/* 
			loop will run until the dataype has found (11) indicating it's the end 
			 of the file
		 */
		while(!endOfFile){
			int type = getDataType(file);

	    	if(type == 0){
	    		printf("int = %d\n", itsAnInt(file));
	    	}
	    	else if(type == 1){
	    		printf("float = %f\n", itsAFloat(file));
	    	}
	    	else if(type == 2){
	    		printf("char = '%c'\n", itsAChar(file));
	    	}
	    	else if (type == 3){
	    		endOfFile = true;
	    	}
		}
	}
	// close file and return 0
	fclose(file);
	return 0;
}