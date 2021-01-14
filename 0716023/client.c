#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 716023
#define SHM_FILE "/tmp/i_love_aries"

int main() {
    int id;

    /************************* TODO 1 *************************/
    // Attach the memory section, the return value is a pointer to
    // the shared memory section
    int shmid = shmget(SHM_KEY, sizeof(int), 0666);
    if (shmid < 0) {
        perror("Failed to access the shared memory");
        exit(-1);
    }
    int *ptr = (int *)shmat(shmid, (void *)0, 0);
    /************************* TODO 1 *************************/

    printf("\033[1;33m[client] The value is %d\033[0m\n", ptr[0]);

    while (1) {
        int cmd;

        printf("\n");
        printf("1: Show the value\n");
        printf("2: Modify the value\n");
        printf("3: Exit\n");
        printf("Enter commands: ");
        scanf("%d", &cmd);

        if (cmd == 1)
            printf("\033[1;33m[client] The value is %d\033[0m\n", ptr[0]);
        else if (cmd == 2) {
            printf("Input new value: ");
            scanf("%d", &ptr[0]);
        } else
            break;
    }

    // detach from shared memory
    shmdt(ptr);
}
