#ifndef libmtu_h
#define libmtu_h

unsigned int mtu_countUTF8(char *bytes);

int mtu_writeFileOpen(char *filename, char *string1, int seek, char *string2);

int mtu_writeFileFopen(char *filename, char *string1, int seek, char *string2);

unsigned int mtu_popen();

int mtu_canNegate(int a);

int mtu_canDivide(int a, int b);

void mtu_qsort(float *list, int n);

char* mtu_lang();

int* mtu_pairSum(int a, int b, ...);

int mtu_printFiles();

#endif
