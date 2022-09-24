#include <stdio.h>
#include <stdlib.h>

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
void   read_entry(struct entry * ent, char *disk, int id);


int main(int argc, char* argv[]){
	
	if(argc<2){
		printf("Use: %s <MBR FILE>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mbr = fopen(argv[1], "r");
	if(mbr==NULL){
		printf("Couldn't read file\n");
		exit(EXIT_FAILURE);
	}

	/* Verificar assinatura */
	unsigned char *sig = malloc(2);
	read_bytes(sig, 0x01fe, 2, 1);
	if((sig[0]<<8|sig[1])!=0x55aa){
		fprintf(stderr,"Error: No MBR signature was found!\n");
		exit(EXIT_FAILURE);
	}
	free(sig);

	printf("Device: %s\n\n", argv[1]);

	/* Verificar entradas */
	int i;
	struct entry *ent= (struct entry *)malloc(sizeof(entry));
	for(i=0; i<4; i++){
		read_bytes(ent, ((int)0x01be)+sizeof(entry)*i, sizeof(entry), 1);
		read_entry(ent, argv[1], i+1);
	}
	free(ent);

	fclose(mbr);
	return EXIT_SUCCESS;
}

size_t read_bytes(void *restrict output, long offset, size_t size, size_t nmemb){
	fseek(mbr, offset, SEEK_SET);
	return fread(output, size, nmemb, mbr);
}

char* get_type(unsigned char type){
	switch(type){
		case 0x00:
			return "empty";
		case 0x83:
			return "linux";
		case 0x82:
			return "swap";
		case 0x05:
			return "extended";
		case 0xef:
			return "uefi";
		case 0xfd:
			return "raid";
		case 0x8e:
			return "lvm";
		case 0x85:
			return "linuxex";
		default:
			return "other/unknown";
	}
}

void   read_entry(struct entry * ent, char* disk, int id){
	unsigned char * ns = ent->nsectors;
	unsigned int nsectors = ns[3]<<24|ns[2]<<16|ns[1]<<8|ns[0];
	if(nsectors!=0){
		printf("%s%d", disk, id);
		if(ent->status==0x80)
			printf(" (ACTIVE)");
		printf("\n");

		unsigned int start = ent->lba[3]<<24|ent->lba[2]<<16|ent->lba[1]<<8|ent->lba[0];
		printf("	Start: %d\n", start);

		unsigned int end = nsectors+start-0x01;
		printf("	End: %d\n", end);
		printf("	Sectors: %d\n", nsectors);

		printf("	Size: ");
		size_t size = nsectors/2;
		if(size>=1048576)
			printf("%dG", size/1048576);
		else if(size>=1024)
			printf("%dM", size/1024);
		else
			printf("%dk", size);
		printf("\n");

		printf("	Id: %x\n", ent->type);
		printf("	Type: %s\n", get_type(ent->type));
	}
}
