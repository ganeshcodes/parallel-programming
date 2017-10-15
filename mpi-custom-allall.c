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
    MPI_Status status;

    // Prepare the data randomly for each process' send buffer
    int i;
    for (i=0;i<S;i++){
        sendBuffer[i] = pid+(i*10);
        recvBuffer[i] = 0;
    }
    int iter=0;
    
    startTime = MPI_Wtime();
    for(;iter<100;iter++){
        // Custom implementation of MPI_Allall using MPI_Send and MPI_Receive
        if(pid!=0){
            // Receive from root process first
            int value;
            MPI_Recv(&value, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            recvBuffer[0] = value;
            printf("P%d received from P0\n",pid);

            // Send message to itself and to all others
            int sendVal;
            for(i=0;i<num_processes;i++){
                if(i==pid){
                    recvBuffer[i] = sendBuffer[i];
                    continue;
                }
                sendVal = sendBuffer[i];
                MPI_Send(&sendVal, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                printf("P0 sent data to P%d\n",i);
            }

            // Receive from all other processes
            for(i=1;i<num_processes;i++){
                if(i!=pid){
                    MPI_Recv(&value, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
                    recvBuffer[i] = value;
                    printf("P%d received from P%d\n",pid,i);
                }
            }
        }

        if(pid==0){
            // Send the message to itself and all other processes
            int sendVal;
            recvBuffer[0] = sendBuffer[0];
            for(i=1;i<num_processes;i++){
                sendVal = sendBuffer[i];
                MPI_Send(&sendVal, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                printf("P0 sent data to P%d\n",i);
            }

        // Receive from all processes
            int value;
            for(i=0;i<num_processes;i++){
                if(i!=pid){
                    //printf("P%d waiting to receive from P%d\n",pid,i);
                    MPI_Recv(&value, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
                    recvBuffer[i] = value;
                    printf("P%d received from P%d\n",pid,i);
                }
            }
        }   
    }
    // Calculate total communication time of all the processes
    endTime = MPI_Wtime();
    totalPerProcess = endTime - startTime;
    printf("per process = %.8f\n",totalPerProcess);
    MPI_Allreduce(&totalPerProcess, &totalTime, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    printf("total time = %.8f\n",totalTime);

    // Finalize the MPI environment.
    MPI_Finalize();
}
