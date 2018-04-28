
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char const *argv[]) {
	char *s = "Hello";
	int i = 1;
// do some computes for the integers
char str[512];
snprintf(str, sizeof str, "Hello #%i", i);
printf("result is '%s'\n", str);
}