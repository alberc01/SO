#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int fdLeds;


void circularCambiLetra();
void parseLetra(int c[]);
void digitos(int c[], int caract);
void asciiConverter(char cadena[]);


int main(int argc, char *argv[]){
 //   int fdLeds;

	fdLeds = open("/dev/leds", O_RDWR);
    if(fdLeds == -1) return -1;

    char cadena[200] ;
    scanf("%[^\n]", cadena);
    
    asciiConverter(cadena);


    close(fdLeds);
    return 0;
}


void asciiConverter(char cadena[]){

	int j=0;
for(j; j< strlen(cadena); j++){
    cadena[j]= tolower(cadena[j]);
    int caract= (int)cadena[j];
    //printf("%d %c \n",caract,cadena[j]);

    int c[3];

    digitos(c, caract);
      

    parseLetra(c);

	circularCambiLetra();
}
}

void circularCambiLetra(){
	printf("cambio de letra\n");
	write(fdLeds,"0",1);
	sleep(1);
	write(fdLeds,"3",1);
	sleep(1);
	write(fdLeds,"23",2);
	sleep(1);
	write(fdLeds,"123",3);
	sleep(1);
}


void parseLetra(int c[]){
	for(int z=0; z<3;z++){
	if(c[z]==0){
		write (fdLeds,"0",1);
		sleep(2);
		write (fdLeds,"0",1);
		printf("se representa un 0\n");
	}
	else if(c[z]==1){
		write(fdLeds,"0",1);
		sleep(2);
		write(fdLeds,"3",1);
		printf("se representa un 1\n");
	}
	else if(c[z]==2){
		write (fdLeds,"0",1);
		sleep(2);
		write(fdLeds,"2",1);
		printf("se representa un 2\n");
	}
	else if(c[z]==3){
		write (fdLeds,"0",1);
		sleep(2);
		write(fdLeds,"23",2);
		printf("se representa un 3\n");
	}
	else if(c[z]==4){
		write (fdLeds,"0",1);
		sleep(2);
		write(fdLeds,"1",1);
		printf("se representa un 4\n");
	}
	else if(c[z]==5){
		write (fdLeds,"0",1);
		sleep(2);
		write(fdLeds,"13",2);
		printf("se representa un 5\n");
	}
	else if(c[z]==6){
		write (fdLeds,"0",1);
		sleep(2);
		write(fdLeds,"12",2);
		printf("se representa un 6\n");
	}
	else if(c[z]==7){
		write (fdLeds,"0",1);
		sleep(2);
		write(fdLeds,"123",3);
		printf("se representa un 7\n");
	}
	else if(c[z]==8){
		write (fdLeds,"3",1);
		sleep(2);
		write(fdLeds,"0",1);
		printf("se representa un 8\n");
	}
	else if(c[z]==9){
		write (fdLeds,"3",1);
		sleep(2);
		write(fdLeds,"1",1);
		printf("se representa un 9\n");
	}
	sleep(2);

}
}

void digitos(int c[], int caract){
	int i=2;
    c[0]=0;
    c[1]=0;
    c[2]=0;
    while(caract != 0){
        int digito=caract % 10 ;
        caract= caract/10;
        c[i]=digito;
        i--;
    }
}