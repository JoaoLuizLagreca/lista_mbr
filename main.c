#include <stdio.h>
#include <stdlib.h>

const char* program_name = "foxdisk";

FILE *mbr;

size_t read_bytes(void *restrict output, long offset, size_t size, size_t nmemb);

int main(int argc, char* argv[]){
	
	if(argc<2){
		printf("Use: %s <MBR FILE>\n", program_name);
		exit(1);
	}

	mbr = fopen(argv[1], "r");
	if(mbr==NULL){
		printf("Couldn't read file\n");
		exit(1);
	}


	fclose(mbr);
	return 0;
}

size_t read_bytes(void *restrict output, long offset, size_t size, size_t nmemb){
	fseek(mbr, offset, SEEK_SET);
	return fread(output, size, nmemb, mbr);
}
