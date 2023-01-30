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
#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4

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
