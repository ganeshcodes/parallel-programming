#include <mpi.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // Get the rank of the process
    int r,i;
    MPI_Comm_rank(MPI_COMM_WORLD, &r);

    int n = atoi(argv[1]);
    int low,high;
    int nbyp = ceil((float)n/p);

    if(r!=0){
        low = (r*nbyp)+1;
        high = (r+1)*nbyp;
        
    }
    if(r==0){
        low = 2;
        high = nbyp;
    }

    int blockSize = high - low + 1;
    int numbers[blockSize];
    int marked[blockSize],j;
    MPI_Status status;

    numbers[0] = low;
    for(i=1;i<=blockSize;i++){
        numbers[i] = low+i;
    }

    for(i=0;i<blockSize;i++){
        if(i!=0&&i%2==0){
            marked[i] = 1;
        }else{
            marked[i] = 0;
        }
    }  

    if(r!=0){
        // Continuously listen for the broadcasted prime number from root process to start with marking multiples
        int currentPrime;
        while(1){
            MPI_Bcast(&currentPrime, 1, MPI_INT, 0, MPI_COMM_WORLD);
            // Mark the multiples of current prime
            for(i=0;i<blockSize;i++){
                if(marked[i]==0 && numbers[i]%currentPrime==0){
                    marked[i]=1;
                }
            }
            // Stop listening for prime numbers from root process if the currentPrime is greater than sqrt of n
            if(currentPrime > sqrt(n)){
                break;
            }
        }
        // Send the local numbers array after marking to root process P0
        for(i=0;i<blockSize;i++){
            if(marked[i]==1){
                numbers[i] = 0;
            }
        }
        // Send size of the numbers array and marked numbers array itself to root process
        MPI_Send(&blockSize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        MPI_Send(numbers, blockSize, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    // First processor finds prime and broadcast it all
    if(r==0){
        for(i=0;i<blockSize;i++){
            // broadcast if the number is not marked, else move on to next number
            if(marked[i]==0){
                MPI_Bcast(&numbers[i], 1, MPI_INT, 0, MPI_COMM_WORLD);
                // Strike out the multiples
                for(j=i+1;j<blockSize;j++){
                    if(numbers[j]%numbers[i]==0){
                        marked[j]=1;
                    }
                }
                // Stop broadcasting if current prime is greater than sqrt of n
                if(numbers[i] > sqrt(n)){
                    break;
                }
            }
        }
        int totalCount = 0;
        // Print the prime numbers in P0
        printf("Root process P0 printing all the prime numbers from itself and all other processes\n");
        for(i=0;i<blockSize;i++){
            if(marked[i]==0){
                printf("%d ",numbers[i]);
                totalCount++;
            }
        }
        // Collect the marked numbers array from all other process after composite number striking
        for(i=1;i<p;i++){
            int numbersSize;
            MPI_Recv(&numbersSize, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
            int partialPrimes[numbersSize];
            MPI_Recv(partialPrimes, numbersSize, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
            for(j=0;j<blockSize;j++){
                if(partialPrimes[j]!=0){
                    printf("%d ",partialPrimes[j]);
                    totalCount++;
                }
            }
        }
        printf("\nTotal number of primes less than %d is %d",n,totalCount);
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}




