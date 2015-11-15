//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gary Muller
// Fall 2015
// CS 420 Assignment 5
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//gets a shared memory segment for an INT Variable type and then spawns a child process that attaches
//that memory and waits for the parent to change the shared value to a non-zero, at which point the 
//child sets it back to zero and terminates, following this the parent will terminate
//
//program will utilize shmget, shmat, shmdt, and shmctl
//
//***specific implementations of these functions mean that this code will likely only run on RHEL***
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Header files - Standard libraries and classes
#include <stdio.h> //standard IO include
#include <sys/types.h> //typedef for unistd
#include <unistd.h> //process control functions
#include <fcntl.h> //file control options
#include <sys/ipc.h> //enables InterProcess Communication
#include <sys/shm.h> //shared memory calls
#include <sys/wait.h> //WIFEXITED(Status) wait for child to terminate

//

//Global Variables and Defines
#define SHM_SIZE sizeof(int)
//

//Function Prototypes

//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Main Declaration
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
main()
{
	//Define Variables
	//shared memory
	int shMemSegID;
	int *shMemSeg;
	//Spawning process
	pid_t childPID = 0;


	//Create and attach a shared memory segment
	//create shared memory segment
	if ((shMemSegID = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666)) < 0)
		//if the returned memory segment id is less than zero(0) (ie -1) run the error commands
		//command contents
		//shared memory segment key id needs to be unique
		//shared memory segment size
		//creation flags -  IPC_CREAT if the segment already exists connect else create, if it exists fail, 0666 rw-rw-rw permissions
	{
		perror("shmget");	//if an error print error
		_exit(-1);			//and exit the program
	}


	//attach the shared memory segment
	if ((shMemSeg = shmat(shMemSegID, NULL, 0)) == (char *)-1)
		//if the attach memory segment returns a value that equals -1 run error commands
		//command contents
		//shared memory segment ID
		//address wher attached shared memory address void pointer
		//creation flags
	{
		perror("shmat");	//if an error print error
		_exit(-1);			//and exit the program
	}



	//OUTPUT: confirm parent created memory segment successfully 
	printf("Parent: Successfully created shared memory segment with shared memory ID # (not segment #) of %d\n\n", shMemSegID);


	//set the shared memory segment to zero(0)
	*shMemSeg = 0;

	//OUTPUT: notify of child spawn, and successful variable set
	printf("Parent: Now spawning a child after setting the shared integer to 0\n");


	//spawn child process with fork
	if ((childPID = fork()) < 0)
	{
		perror("fork");		//if an error print error
		_exit(-1);			//and exit the program
	}



	if (childPID != 0)
	{
		printf("Parent: My pid is %ul, spawned a child with pid of %ul; please enter an integer to be stored in shared memory: ", getpid(), childPID);
		scanf("%d", shMemSeg);

		printf("spinning, parent");//debug
		//spin while waiting for zero
		while (shMemSeg != 0);
		printf("done spinning, parent");//debug
		//print that zero has been set
		printf("Parent: the child has re-zeroed our shared integer\n");

		//Detach the Memory segment
		if (shmdt(shMemSeg) == -1)
			//if the attach memory segment returns a value that equals -1 run error commands
			//command contents
			//shared memory address void pointer
		{
			perror("shmdt");	//if an error print error
			_exit(-1);			//and exit the program
		}

		int rtrn;

		if ((rtrn = shmctl(shMemSegID, IPC_RMID, 0)) == -1)
			//shared memory control
			//command contents
			//shared memory segment ID
			//control command
			//
		{
			perror("shmctl");	//if an error print error
			_exit(-1);			//and exit the program
		}//if

		printf("Parent: Child terminated; parent successfully removed segment whose ID # was %ul\n\n", shMemSegID);
	}
	else if (childPID == 0)
	{
		printf("\n\tChild: My pid is %ul; my parent's pid is %ul; the shared integer value is currently 0; I'll spin until it's not 0\n\n", getpid(), getppid());
		//print the statement that it is a child and the PID

		while (*shMemSeg == 0); //spin while waiting for the shared memory to change from 0

		printf("Child: The value in the shared integer is now %d\n", *shMemSeg);

		(*shMemSeg) = 0;
		printf("%d, Child", *shMemSeg);//debug
		printf("Child process terminating\n");

	}

} //end of main()