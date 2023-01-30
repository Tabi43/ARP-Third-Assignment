#include "./../include/processB_utilities.h"
#include "./../include/utilities.h"

/*Shared memory*/
int shm_fd; 
rgb_pixel_t * ptr;

/*Semaphore*/
sem_t * sem_id1;
sem_t * sem_id2;

/*Variable to store the execution mode (normal, server or client)*/
int mode;

int main(int argc, char const *argv[])
{
    if(mode == 2 || mode == 3) {
        char address[256];
        int port;
        printf("Enter the address of the companion application: ");
        scanf("%s", address);
        printf("Enter the port of the companion application: ");
        scanf("%d", &port);
        // create a socket and connect to the companion application
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        inet_pton(AF_INET, address, &serv_addr.sin_addr);
        connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    }

    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    //Open semaphore
    sem_id1 = sem_open(sem_path_1, 0);
    sem_id2 = sem_open(sem_path_2, 0);

    // create the shared memory object
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SHM_SIZE);

    /* memory map the shared memory object */
    ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Infinite loop
    while (TRUE) {       

        /*Wait to enter in the critic section*/
        sem_wait(sem_id2);

        /*Create a temporal pointer*/
        rgb_pixel_t * tmp_ptr = ptr+1;

        int count = 1;
        int sum_x = 0;
        int sum_y = 0;

        for(int i = 0; i < SM_WIDTH; i++){            
            for(int j = 0; j < SM_HEIGHT; j++){   
                
                rgb_pixel_t * p = tmp_ptr++;
                int r = p->red;
                int g = p->green;
                int b = p->blue;
                int a = p->alpha;

                /*Searching colored pixel*/   
                if((r < 255) || (g < 255) || (b < 255)){                   
                    sum_x += i;
                    sum_y += j;
                    count++;
                }
            }            
        }        

        int x = sum_x/count;
        int y = sum_y/count;

        float scale_x = SM_WIDTH/COLS;
        float scale_y = SM_HEIGHT/LINES;      
        
        mvaddch(y/scale_y, x/scale_x, '0');
        refresh();        

        /*Signal exiting from the critic section*/
        sem_post(sem_id1);  

        // Get input in non-blocking mode
        int cmd = getch();

        // If user resizes screen, re-draw UI...
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }
        
    }

    endwin();
    return 0;
}
