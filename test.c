#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char*argv[]){
	FILE* f;
	if((f = fopen(argv[1], "r")) == NULL){
		fprintf(stderr, "Can't open a file.\n");
		return 1;
	}
	char S[100];
	char storage[1000] = "helo";
	while(!feof(f)){
		fgets(S, 98, f);
		puts(S);
		strcat(storage,S);	
	}

	//printf("\n%s\n",*message);
	
	return 0;
}
