#include <stdio.h>
#include <bmpfile.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>
#include <bmpfile.h>
#include <semaphore.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

/*Socket lib*/
#include <sys/socket.h>
#include <netinet/in.h>

/*CONV*/
#define UP "u"
#define DOWN "d"
#define RIGHT "r"
#define LEFT "l"

/*Parameters*/
const int SM_WIDTH = 1600;
const int SM_HEIGHT = 600;
const int DEPTH = 4;
const int RADIUS = 30;

int n_curses_width = 80;
int n_curses_height = 30;

const int COLOR_SEG = SM_WIDTH*SM_HEIGHT;
/*COLOR SEGMENT
  0: Blue
  1: Green
  2: Red
*/  
const int SM_FACTOR = 20;
const int BMP_CIRC_RADIUS = 60;

#define shm_name "/AV"

const int SHM_SIZE = SM_WIDTH*SM_HEIGHT*4;

#define sem_path_1 "/sem_AV_1" //Sem_procuder
#define sem_path_2 "/sem_AV_2" //Sem_consumer

int write_log(char * path, char * tag, char * msg){
  /*Try to write the msg log to the desired path*/
  char buffer[512];
  int DIM_BUFFER = 512;
  snprintf(buffer, DIM_BUFFER, "\n[%s] : %s\0",tag , msg);
  int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if(fd < 0){
    perror("opening log file");
    return -1;
  }else{   
    int n = str_len(buffer);
    if(write(fd, buffer, n) != n){
      perror("writing log file");
      return -1;
    }else{
      close(fd);
      return 0;
    }
  } 
}

int str_len(char * str){
	int i ;
	for(i = 0; i < 1024; i++){
		if(str[i] == '\0') return i; 
	}
	return i;
}