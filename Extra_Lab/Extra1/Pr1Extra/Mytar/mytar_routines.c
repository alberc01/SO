#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;


unsigned short fletcher16_file(FILE *file){
    int c;
    unsigned short sum1 = 0;
    unsigned short sum2 = 0;
    while ((c = getc(file)) != EOF)
    {
        sum1 = (sum1 + (unsigned short)c) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    return (sum2 << 8) | sum1;
}
/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE *origin, FILE *destination, int nBytes,unsigned short *crc)
{
	int ini=0;
	int c;
//	unsigned short crcaux;
    unsigned short sum1 = 0;
    unsigned short sum2 = 0;
	  while ((c = getc(origin)) != EOF && ini < nBytes )  {
		  putc(c,destination);
	        sum1 = (sum1 + (unsigned short)c) % 255;
	        sum2 = (sum2 + sum1) % 255;
	        ini++;
	    }
//	while(data != EOF && ini < nBytes ){
//		putc(data,destination);
//		data=getc(origin);
//		ini++;
//	}
	if(ini!= 0){
		(*crc)=(sum2 << 8) | sum1;
		return ini;
	}
	else return -1;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	char *cadena=malloc(sizeof(char));
	char* returned= malloc(sizeof(char*));
	if(	fread(cadena,sizeof(char),1,file)>0){
		while(strcmp(cadena,"\0")){
			strcat(returned,cadena);
			fread(cadena,sizeof(char),1,file);
		}
		free(cadena);
		return returned;
	}
	else{
		return NULL;

	}
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry* array=NULL;

	int nr_files=0;
	if(fread(&nr_files,sizeof(int),1,tarFile)>0){
		/* Allocate memory for the array */
		array=malloc(sizeof(stHeaderEntry) *nr_files);
		for(int i= 0 ; i< nr_files; i++){
			array[i].name=loadstr(tarFile);
			fread(&array[i].size,sizeof(int),1,tarFile);
			fread(&array[i].crc,sizeof(unsigned short),1,tarFile);
		}
		/* Store the number of files in the output parameter */
		(*nFiles)=nr_files;
		return array;
	}
	return NULL;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE *destinationFile, *inputfile;
	destinationFile= fopen(tarName,"w");
	unsigned short crcaux=0;
	stHeaderEntry* array=malloc(sizeof(stHeaderEntry)*nFiles);
	int filespace=0;
	for(int i=0 ; i< nFiles;i++){
		array[i].name=malloc(strlen(fileNames[i])+1);
		strcat(array[i].name,fileNames[i]);
		filespace+=(strlen(fileNames[i])+1);
	}
	long int desp= (sizeof(unsigned short)*nFiles)+ filespace + sizeof(int) + nFiles*sizeof(unsigned int);
	fseek(destinationFile,desp,SEEK_CUR);

	for(int j = 0 ; j< nFiles; ++j){
		inputfile=fopen(fileNames[j],"r");
		int aux = copynFile(inputfile,destinationFile,INT_MAX, &crcaux);
		if(aux==-1){
			return EXIT_FAILURE;
		}
		fclose(inputfile);
		desp+=aux;
		array[j].size=aux;
		array[j].crc= crcaux;
	}

	fseek(destinationFile,-desp,SEEK_CUR);
	fwrite(&crcaux,sizeof(unsigned short),1,destinationFile);// addcrc
	fwrite(&nFiles,sizeof(int),1,destinationFile);
	for(int z= 0; z < nFiles;z++){
		fwrite(array[z].name,sizeof(char)*strlen(array[z].name),1,destinationFile);
		putc('\0',destinationFile);
		fwrite(&array[z].size,sizeof(int),1,destinationFile);
		fwrite(&array[z].crc,sizeof(unsigned short),1,destinationFile);
		printf("Adding file %s , size %d Bytes, CRC %d \n", array[z].name, array[z].size, array[z].crc);
	}
	for(int w=0;w<nFiles;w++){
		free(array[w].name);
	}
	fclose(destinationFile);
	free(array);
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 */
int
extractTar(char tarName[])
{
	FILE * tarfile;
			if(tarfile=fopen(tarName,"r")==NULL){
				printf("Mal");

			}
	int *nFiles=malloc(sizeof(int));
	unsigned short crc=0;
	stHeaderEntry* array=readHeader(tarfile,nFiles);
	if(array!=NULL){
		long int desp= (*nFiles)*sizeof(unsigned short)+ sizeof(int) + (*nFiles)*sizeof(unsigned int);
		for(int j=0 ; j<(*nFiles);j++){
			desp+=(strlen(array[j].name)+1);
		}
		int aux=0;
		FILE * newfile;
		for(int i=0; i< (*nFiles ); i++){
			newfile= fopen(array[i].name,"w");
			desp+=aux;
			fseek(tarfile,desp,SEEK_SET);
			aux=copynFile(tarfile,newfile,array[i].size,&crc);
			printf("Creating file %s , size %d Bytes, CRC %d \n", array[i].name,array[i].size,array[i].crc);
			if(crc!= array[i].crc){
				printf("Los CRC son distintos %d \n",crc);
			}
			else{
				printf("CRC of extracted file %d. File is OK. \n",array[i].crc);
			}
			fseek(tarfile,-desp,SEEK_CUR);
			fclose(newfile);
		}

		for(int w=0;w<(*nFiles);w++){
			free(array[w].name);
		}
		free(array);
		free(nFiles);
		return EXIT_SUCCESS;
	}
	else{
		return EXIT_FAILURE;

	}
}
