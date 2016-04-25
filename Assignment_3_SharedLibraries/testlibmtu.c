#include <stdio.h>
#include <limits.h>
#include <dirent.h>
#include <stdbool.h>
#include "libmtu.h"

int main(){
	printf("Testing that mtu_countUTF8(\"∞ hello\") returns 7. ");
	char *why = "∞ hello";
	if(mtu_countUTF8(why) != 7) printf("Failure\n");
	else printf("Success\n");
	//if(mtu_countUTF8(why) != 10) printf("failure on count_UTF8, testing for blank string with length 10\n");
	//else printf("Success\n");

	printf("-------------------\n");

	printf("Testing that mtu_writeFileOpen() returns 1. ");
	char *fileName = "hi.txt";
	if(mtu_writeFileOpen(fileName, "NoNoNo", 6, "YesYesYes") != 1) printf("Failure.\n");
	else printf("Success.\n");
	printf("Testing that mtu_writeFileFopen() returns 1. ");
	char *newFile = "Neil.txt";
	if(mtu_writeFileFopen(newFile, "My name is neil", 15, "last name Culbertson") != 1) printf("Failure.\n");
	else printf("Success.\n");

	printf("-------------------\n");
	//mtu_popen();

	printf("Testing mtu_pairSum(1, 2, 0, 0) returns {3}. ");
	int *newPtr =  mtu_pairSum(1, 2, 0, 0);
	if(newPtr[0] != 3) printf("Failure.\n");
	else printf("Success.\n");
	printf("Testing mtu_pairSum(1, 2, 3, 4, 1000, 528, -2000, 2000, 0, 0) returns {3, 7, 1528, 0}. ");
	int *ptr = mtu_pairSum(1, 2, 3, 4, 1000, 528, -2000, 2000, 0, 0);
	if(ptr[0] != 3 && ptr[1] != 7 && ptr[2] != 1528 && ptr[3] != 0) printf("Failure.\n");
	else printf("Success.\n");
	printf("Testing mtu_pairSum(0, 0) returns {0}. ");
	int *ptr1 = mtu_pairSum(0, 0);
	if(ptr1[0] != 0) printf("Failure.\n");
	else printf("Success.\n");
	printf("Testing mtu_pairSum(1, 2, 10, 100, 0, 0, 100, 100) returns {3, 110, 0}. ");
	int *ptr2 = mtu_pairSum(1,2,10,100,0,0,100,100);
	if(ptr2[0] != 3 && ptr2[1] != 110 && ptr2[2] != 0) printf("Failure.\n");
	else printf("Success.\n");

	printf("-------------------\n");
	
	printf("Testing mtu_qsort({2.3, -4.5, 6.7, -1.2, 4.5, 0.1}, 6) returns {0.1, 1.2, 2.3, 4.5, 4.5, 6.7}. ");
	bool failure = false;
	float theList[] = {2.3, 4.5, 6.7, -1.2, -4.5, 0.1};
	float sorted[]  = {-4.5, -1.2, 0.1, 2.3, 4.5, 6.7};
	mtu_qsort(theList, 6);
	for(int i = 0; i < 6; i += 1){
		if(theList[i] != sorted[i]) failure = true;
	}
	if(failure) printf("Failure.\n");
	else printf("Success.\n");

	printf("-------------------\n");
	
	printf("Testing mtu_lang() does not return null. ");
	if(mtu_lang() == NULL) printf("Failure.\n");
	else printf("Success.\n");

	printf("-------------------\n");

	printf("Testing mtu_canNegate(-2147483648) returns 0. ");
	if(mtu_canNegate(-2147483648) == 0) printf("Success.\n");
	else printf("It returned %d. Failure\n", mtu_canNegate(-2147483648));
	printf("Testing mtu_canNegate(5) returns 0. ");
	if(mtu_canNegate(5) != 1) printf("It returned %d. Failure.\n", mtu_canNegate(5));
	else printf("Success.\n");
	
	printf("-------------------\n");

	printf("Testing mtu_canDivide(1, 0) returns 0. ");
	if(mtu_canDivide(1, 0) == 0) printf("Success\n");
	else printf(" It returned %d. Success\n", mtu_canDivide(1, 0) == 1);
	printf("Testing mtu_canDivide(1, 5) returns 1. ");
	if(mtu_canDivide(1, 5) == 1) printf("Success\n");
	else printf("It returned %d. Failure.\n", mtu_canDivide(1, 5));

	printf("-------------------\n");

	printf("Testing mtu_printFiles() does not return 0. ");
	if(mtu_printFiles() != 0) printf("Success\n");
	else printf("It didn't return 0. Failure.\n");

	printf("Testing that mtu_popen() doesn't return null. ");
	if(mtu_popen() != 0) printf("Success.\n");
	else printf("Failure.\n");

	return 0;
}