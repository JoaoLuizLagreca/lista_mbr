#include <stdio.h>
#include <stdlib.h>

const char* program_name = "foxdisk";

FILE *mbr;
typedef struct entry{
	unsigned char status;
	unsigned char first_abs[3];
	unsigned char type;
	unsigned char last_abs[3];
	unsigned char lba[4];
	unsigned char nsectors[4];
	
}entry;

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

	/* Verificar assinatura */
	unsigned char *sig = malloc(2);
	read_bytes(sig, 0x01fe, 2, 1);
	if((sig[0]<<8|sig[1])!=0x55aa){
		fprintf(stderr,"Error: No MBR signature was found!\n");
		exit(EXIT_FAILURE);
	}
	free(sig);

	fclose(mbr);
	return 0;
}

size_t read_bytes(void *restrict output, long offset, size_t size, size_t nmemb){
	fseek(mbr, offset, SEEK_SET);
	return fread(output, size, nmemb, mbr);
}
