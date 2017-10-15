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
        printf("Initialising processor %d out of %d processors\n", pid, num_processes);

        // Initialise send and receive buffer for all the processes
        int S = atoi(argv[1]);
        int* sendBuffer = (int*)malloc(sizeof(int) * S);
        int* recvBuffer = (int*)malloc(sizeof(int) * S);
        double startTime, endTime, totalPerProcess, totalTime;

        // Prepare the data randomly for each process' send buffer
        int i;
        for (i=0;i<S;i++){
            sendBuffer[i] = pid+(i*10);
            recvBuffer[i] = 0;
        }   

        // Calculate the time taken for each process
        startTime = MPI_Wtime();
        for(i=0;i<100;i++){
            MPI_Alltoall(sendBuffer, 1, MPI_INT, recvBuffer, 1, MPI_INT, MPI_COMM_WORLD);
        }
        endTime = MPI_Wtime();
        totalPerProcess = endTime - startTime;
        printf("totalPerProcess = %.8f\n",totalPerProcess);

        // Calculate total communication time of all the processes
        MPI_Allreduce(&totalPerProcess, &totalTime, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        printf("totalTime = %.8f",totalTime);
        // Finalize the MPI environment.
        MPI_Finalize();
    }
