#include "shmem.h"

#include <sys/types.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMEM_KEY1 425678
int* first_is_new = NULL;
#define SHMEM_KEY2 425679
movement* first = NULL;
#define SHMEM_KEY3 425680
movement* second = NULL;



void shmem_init(bool init_move) {
	int shmid;
	
	//Create the segment.
	if ((shmid = shmget(SHMEM_KEY1, sizeof(int), IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(1);
	}
	//Now we attach the segment to our data space.
	if ((first_is_new = (int*)shmat(shmid, NULL, 0)) == (int*) -1) {
		perror("shmat");
		exit(1);
	}
	
	//Create the segment.
	if ((shmid = shmget(SHMEM_KEY2, sizeof(movement), IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(1);
	}
	//Now we attach the segment to our data space.
	if ((first = (movement*)shmat(shmid, NULL, 0)) == (movement*) -1) {
		perror("shmat");
		exit(1);
	}
	
	//Create the segment.
	if ((shmid = shmget(SHMEM_KEY3, sizeof(movement), IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(1);
	}
	//Now we attach the segment to our data space.
	if ((second = (movement*)shmat(shmid, NULL, 0)) == (movement*) -1) {
		perror("shmat");
		exit(1);
	}
	
	if(init_move) {
		*first = movement(0,0,0,0);
		*first_is_new = true;
	}
}

movement shmem_get() {
	if(first_is_new == NULL) {
		cout<<"Use shmem_init() before shmem_get()."<<endl;
		exit(1);
	}
	
	if(*first_is_new) {
		return *first;
	} else {
		return *second;
	}
	
	
}

void shmem_set(movement& m) {
	if(first_is_new == NULL) {
		cout<<"Use shmem_init() before shmem_set()."<<endl;
		exit(1);
	}
	
	if(*first_is_new) {
		*second = m;
		//first_is_new = false;
		(void)__sync_fetch_and_and(first_is_new, 0);
	} else {
		*first = m;
		//shm->first_is_new = false;
		(void)__sync_fetch_and_or(first_is_new, 1);
	}
	
}

