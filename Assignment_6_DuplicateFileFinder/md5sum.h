// Neil Culbertson

#define _GNU_SOURCE
#include <stdio.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <stdint.h>

#ifndef MD5SUM_H
#define MD5SUM_H

struct Hashes{
	unsigned char hash[MD5_DIGEST_LENGTH];
	char *filename;
	char dir[1024];
};

struct Hashes hashes[1750];
char calculateMD5sum(char *filename, int index);
void put_MD5sum_In_Array(unsigned char md5sum[], char *filename);
void printMD5sum(unsigned char md5sum[], char *filename);
void printMD5sum_signed(char md5sum[], char *filename);
void put_MD5sum_In_struct(char *fname);
void findDuplicates(unsigned char **files);
void fillHashArray(unsigned char md5sum[]);
void printMD5_array(int numberOfFiles, unsigned char **sum);

#endif