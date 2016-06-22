#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include "memory.h"
#include "cpu.h"
#include "dump.h"
#include <stdio.h>


void simulationdump(cpu_core core, int clock){

FILE *fd;
	/* apre il file in scrittura */
  fd=fopen("dump.txt", "w");
  if( fd==NULL ) {
    perror("Errore in apertura del file");
    exit(1);
  }	    
		fprintf(fd,"\n ~ ~ ~ ~ ~ CLOCK CYCLES ~ ~ ~ ~ ~\n");
		fprintf(fd,"\t\t %d", clock);
		
		fprintf(fd,"\n  ~ ~ ~ ~ ~ REGISTER DUMP ~ ~ ~ ~ ~ \n");
		for (int32_t x = 0; x < 8; x++) {
		    fprintf(fd, "$%d:\t0x%08x\t\t$%d:\t0x%08x\t\t$%d:\t0x%08x\t\t$%d:\t0x%08x\n"
		          , x, core.registers[x]
		          , x + 8, core.registers[x + 8]
		          , x + 16, core.registers[x + 16]
		          , x + 24, core.registers[x + 24]);
		}
		fprintf(fd,"\n\n ---------------------------------\n\n ---------------------------------\n\n");


/*
		fprintf(fd,"\n ~ ~ ~ ~ ~ MEMORY DUMP ~ ~ ~ ~ ~\n");
		
	    	int *mp = 0x10000000;       
		while (*mp < 10002000){
			fprintf(fd,"0x%d" &mp);
			*mp++;
		} 
*/
	/* chiude il file */
  fclose(fd);
    

return;

}




