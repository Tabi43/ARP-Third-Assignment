#include "./../include/processA_utilities.h"
#include "./../include/utilities.h"

/*Bitmap object*/
bmpfile_t * bmp;

/*Data type for define a BGRA pixel*/
rgb_pixel_t pixel = {255, 0, 0, 0}; //Color of the circle
rgb_pixel_t empty_pixel = {255, 255, 255, 0}; //White pixel

/*Shared memory*/
int shm_fd; 
rgb_pixel_t * ptr;

/*Print counter*/
int print_counter = 0;

/*Semaphores*/ 
sem_t * sem_id1;
sem_t * sem_id2;

/*Socket descriptor for client or server mode*/
int sockfd;

/*IT*/
int mode;
int portno;
int sockfd;
int clilen;
int newsockfd;

struct sockaddr_in serv_addr; /*Server address*/
struct sockaddr_in cli_addr; /*Server address*/
struct hostent *server;

/* Save the current bitmap object to 
out directory starting from 0 */
bool take_snapshot(){
    char path[20];

    snprintf(path, 20, "out/%d.bmp", print_counter);
    print_counter += 1;

    bmp_save(bmp, path);   
}

/*Draw a colored circle getting the bitmap object and
the center of the circle*/
void draw__colored_circle_bmp(bmpfile_t * bmp, int xc, int yc){   
  for(int x = -RADIUS; x <= RADIUS; x++) {
        for(int y = -RADIUS; y <= RADIUS; y++) {
        // If distance is smaller, point is within the circle
            if(sqrt(x*x + y*y) < RADIUS) {
                /*
                * Color the pixel at the specified (x,y) position
                * with the given pixel values
                */
                bmp_set_pixel(bmp, xc + x, yc + y, pixel);
            }
        }
    }
}

/*Draw a white circle getting the bitmap object and
the center of the circle*/
void draw__empty_circle_bmp(bmpfile_t * bmp, int xc, int yc){
    for(int x = -RADIUS; x <= RADIUS; x++) {
        for(int y = -RADIUS; y <= RADIUS; y++) {
        // If distance is smaller, point is within the circle
            if(sqrt(x*x + y*y) < RADIUS) {
                /*
                * Color the pixel at the specified (x,y) position
                * with the given pixel values
                */
                bmp_set_pixel(bmp, xc + x, yc + y, empty_pixel);
            }
        }
    }
}

void load_bmp_to_shm(bmpfile_t * bmp, rgb_pixel_t * ptr){
    int pos = 0;   

    /*Sem wait*/
    sem_wait(sem_id1);

    /*Loading pixel*/
    for(int i = 0; i < SM_WIDTH; i++){
        for(int j = 0; j < SM_HEIGHT; j++){
            pos = (i*SM_WIDTH)+j+1; 
            ptr++;             
            /*BGRA*/
            rgb_pixel_t * tmp_p = bmp_get_pixel(bmp,i,j);
            int b = tmp_p->blue;
            int g = tmp_p->green;
            int r = tmp_p->red;
            int a = tmp_p->alpha;
            rgb_pixel_t alfio = {b,g,r,a};          
            *ptr = alfio;                
        }
    }

    /*sem signal*/
    sem_post(sem_id2);    
}

/*Reset function to clear all bitmap obj
after a resize of the window*/
void reset_bmp(bmpfile_t * bmp) {
    for(int i = 0; i < SM_WIDTH; i++){
        for(int j = 0; j < SM_HEIGHT; j++){
               bmp_set_pixel(bmp, i, j, empty_pixel);
        }
    }
}

int main(int argc, char *argv[]) {
    
    if(argc > 0){
        /*Check args*/

        mode = atoi(argv[1]);

        switch (mode) {
            case 1:{
                /*Normal mode*/
                
            } break;
            case 2:{
                /*Server mode*/

                portno = atoi(argv[2]);
                /*Check on port number*/

                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd < 0) error("ERROR opening socket");
                bzero((char *) &serv_addr, sizeof(serv_addr));

                serv_addr.sin_family = AF_INET;
                serv_addr.sin_addr.s_addr = INADDR_ANY;
                serv_addr.sin_port = htons(portno);

                if (
                    bind(
                        sockfd, 
                        (struct sockaddr *) &serv_addr,
                        sizeof(serv_addr)
                    )
                < 0) error("ERROR on binding");

                listen(sockfd,5);
                clilen = sizeof(cli_addr);
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0) error("ERROR on accept");

                /*Ready*/

            } break;
            case 3:{
                /*Client mode*/                
                
                portno = atoi(argv[2]);
                /*Check on port number*/

                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd < 0) error("ERROR opening socket");

                server = gethostbyname(argv[3]);

                if (server == NULL) {
                    fprintf(stderr,"ERROR, no such host\n");
                    exit(0);
                }
                bzero((char *) &serv_addr, sizeof(serv_addr));
                serv_addr.sin_family = AF_INET;
                bcopy(
                    (char *)server->h_name,
                    (char *)&serv_addr.sin_addr.s_addr,
                    server->h_length
                );
                serv_addr.sin_port = htons(portno);
                if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");

                /*Ready*/                

            } break;

            default:{
                
            }break;
        }

    }

    // Normal execution, use keyboard input
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    //Open semaphores
    sem_id1 = sem_open(sem_path_1, 0);
    sem_id2 = sem_open(sem_path_2, 0);

    // Initialize UI
    init_console_ui();

    // Instantiate bitmap
    bmp = bmp_create(SM_WIDTH, SM_HEIGHT, DEPTH);

    // create the shared memory object
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SHM_SIZE);

    /* memory map the shared memory object */
    ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Infinite loop
    while (TRUE) {
        // Get input in non-blocking mode
        int cmd = getch();
        float scale_x = SM_WIDTH/(COLS-BTN_SIZE_X);
        float scale_y = SM_HEIGHT/LINES;

        // If user resizes screen, re-draw UI...
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
                reset_bmp(bmp);
                float scale_x = SM_WIDTH/(COLS-BTN_SIZE_X);
                float scale_y = SM_HEIGHT/LINES;
                draw__colored_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                load_bmp_to_shm(bmp, ptr);
            }
        }

        // Else, if user presses print button...
        else if(cmd == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                if(check_button_pressed(print_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Print button pressed");

                    /*Call print to file function*/
                    take_snapshot();

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }

        if(mode == 2){
            /*Server mode*/
            char cmd;
            if(read(newsockfd,cmd,1) != 1){                
                int t = atoi(cmd);

                switch (t) {
                    case UP:{
                        int cmd_tmp = KEY_UP;
                        draw__empty_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        move_circle(cmd_tmp);
                        draw__colored_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        draw_circle();
                    } break;
                    case DOWN:{
                        int cmd_tmp = KEY_DOWN;
                        draw__empty_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        move_circle(cmd_tmp);
                        draw__colored_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        draw_circle();
                    } break;
                    case RIGHT:{
                        int cmd_tmp = KEY_RIGHT;
                        draw__empty_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        move_circle(cmd_tmp);
                        draw__colored_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        draw_circle();
                    } break;
                    case LEFT:{
                        int cmd_tmp = KEY_LEFT;
                        draw__empty_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        move_circle(cmd_tmp);
                        draw__colored_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        draw_circle();
                    } break;
                    
                    /*Sync with Shared memory image*/
                    load_bmp_to_shm(bmp, ptr);
                    
                }
            }else{
                /*Problem*/
            }
        }

        // If input is an arrow key, move circle accordingly...
        else if(cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN) {            

            switch (mode) {
                case 1:{
                    /*Normal mode*/
                    draw__empty_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                    move_circle(cmd);
                    draw__colored_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                    draw_circle();
                    /*Sync with Shared memory image*/
                    load_bmp_to_shm(bmp, ptr);
                }break;                
                case 3:{
                    /*Client mode*/
                    char msg;
                    switch (cmd) {
                        case KEY_UP:{
                            cmd = UP;
                        } break;
                        case KEY_DOWN:{
                            cmd = DOWN;
                        } break;
                        case KEY_RIGHT:{
                            cmd = RIGHT;
                        } break;
                        case KEY_LEFT:{
                            cmd = LEFT;
                        } break;                       
                    }
                    if(write(sockfd,msg,1) != 1){
                        error("ERROR writing to socket");
                    }else{
                        draw__empty_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        move_circle(cmd);
                        draw__colored_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
                        draw_circle();
                        /*Sync with Shared memory image*/
                        load_bmp_to_shm(bmp, ptr);
                    }                    
                }break;
            }

           
        }
    }
    
    endwin();
    return 0;
}
