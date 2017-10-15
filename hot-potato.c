#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    // Get the rank of the process
    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    // Print initialization message
    //printf("Initialising processor %d out of %d processors\n", pid, num_processes);

    int i,destination,sender,msgSize,min,max,increment;
    double startTime,endTime,totalTime,latency;
    MPI_Status status;

    // Data size in bytes (512 bytes or 128KB)
    int dataSize = atoi(argv[1]);
    int maxNumbers = dataSize/sizeof(int);
    int data[maxNumbers];

    for(i=0;i<maxNumbers;i++){
        data[i] = i+1;
    }

    // All process except root process gets blocked
    if (pid != 0) {
        MPI_Recv(data, maxNumbers, MPI_INT, pid-1, 1, MPI_COMM_WORLD, &status);
    }

    // P0 executes send first followed by others
    destination = (pid+1)% num_processes;
    startTime = MPI_Wtime();
    MPI_Send(data, maxNumbers, MPI_INT, destination, 1, MPI_COMM_WORLD);
    //printf("P%d sent data to P%d\n", pid,destination);

    // P0 receives from last process
    if (pid == 0) {
        MPI_Recv(data, maxNumbers, MPI_INT, num_processes-1, 1, MPI_COMM_WORLD, &status);
        //printf("Root process (P%d) receives data from last process(P%d)\n", pid,num_processes);
        // Overall execution time of all transfers
        endTime = MPI_Wtime();
        totalTime = endTime - startTime;
        printf("%.8f\n",totalTime);
    }
        
        
    // Finalize the MPI environment.
    MPI_Finalize();
}