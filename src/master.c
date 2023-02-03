#include "./../include/utilities.h"

/*Client/Server TCP variable*/
int mode = 0;   /*Variable to store the execution mode (normal, server or client)*/
char address[256] = "";
int port = 0;

int spawn(const char * program, char * arg_list[]) {

  pid_t child_pid = fork();

  if(child_pid < 0) {
    perror("Error while forking...");
    return 1;
  }

  else if(child_pid != 0) {
    return child_pid;
  }

  else {
    if(execvp (program, arg_list) == 0);
    perror("Exec failed");
    return 1;
  }
}

int main() {  

  char buffer_A[512];
  int DIM_BUFFER = 512;

  snprintf(buffer_A, DIM_BUFFER, "./bin/processA %d %d %s" ,mode, port, address);
  
  char * arg_list_A[] = {"/usr/bin/konsole", "-e", "./bin/processA", NULL };
  char * arg_list_B[] = {"/usr/bin/konsole", "-e", "./bin/processB", NULL };

  /*Istantiate Shared Memory*/
  int shm_fd;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  if(shm_fd == 1){
    printf("Shared memory segment failed\n");
    exit(1);
  }

  /*Istantiate Semaphore for Producer*/
  sem_t * sem_id1 = sem_open(sem_path_1, O_CREAT, S_IRUSR | S_IWUSR, 1);
  sem_init(sem_id1, 1, 1);

  /*Istantiate Semaphore for Consumer*/
  sem_t * sem_id2 = sem_open(sem_path_2, O_CREAT, S_IRUSR | S_IWUSR, 1);
  sem_init(sem_id2, 1, 0);
  
  pid_t pid_procA = spawn("/usr/bin/konsole", arg_list_A);
  pid_t pid_procB = spawn("/usr/bin/konsole", arg_list_B);

  int status;
  waitpid(pid_procA, &status, 0);
  waitpid(pid_procB, &status, 0);

  printf ("Main program exiting with status %d\n", status);

  return 0;
}

