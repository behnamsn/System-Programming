#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

/*
This program is an example of a producer and consumer simulation based on threads.
Threads are exploited to save the resources and processes.
This program gets the number of producers and conusmers (equal) and then 
update a repository of 10 different items. Each item initialized with 0 quantity. 
The repository will be updated whenever the requested quantity is not sufficient.
On every update, a random number of quantity (0-10) will be added to the repository.
The requests are being made on a random item and random quantity (0-100) 
*/

int rand_item=0; 
int rand_to_be_served=0;
int ready=0; ///declaring an indicator to specify if the quantity of the item is available or not
/////declaring the mutex to prevent race condition////
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t update=PTHREAD_COND_INITIALIZER; ///declaring condition variable to use
                                                /// multi threads in a producer and consumer approach

void* producer(void* arg){
    int rand_q[10];
    int *repos=(int*) arg; /// declaring an integer pointer from the argument of the thread function
    while(1){ /////creating a loop to make a non-stop production process 
        for(int i=0; i<10;i++){ /////creating a loop for generating the 10 random numbers for each item
            rand_q[i]=1+rand()%10;
            sleep(2); ///sleeping 2 seconds after generation a random number for each single item
        }
        printf("\n\n");
        printf("Updating:\n");
        ///// locking the resources to let the other threads writing on the ready indicator
        pthread_mutex_lock(&mutex); 
        printf("REPOSITORY STATISTICS\n");
        for(int i=0;i<10;i++){
            repos[i]+=rand_q[i];
            printf("Item %d = %d\n",i+1,*(repos+i));
        }
        /////condition if the quantity that has been requested are enough 
        if(repos[rand_item]>=rand_to_be_served){
            ready++; /// increment the indicator to let the consumber
                    /// know that the quantity is availbale now
            pthread_cond_signal(&update); /// using condition variable to inform the consumer
        }
        pthread_mutex_unlock(&mutex); ///unclock the resources for other consumers
        printf("\n\n");
     }
}

void* consumer(void* arg){
    int *repos=(int*) arg; /// declaring an integer pointer from the argument of the thread function
    while(1){
        pthread_mutex_lock(&mutex); /// lock the resources to prevent race conddition 
        while(ready==0){ /// using a loop to wait for the producer to produce the requested quantity
            pthread_cond_wait(&update,&mutex); /// using a condition variable to  wait for the producer
            printf("\nDemand has been Served; Item %d had %d quantities and demand was %d\n\n",rand_item+1,repos[rand_item],rand_to_be_served);
            repos[rand_item]-=rand_to_be_served; /// updating the repository using an array pointer
            printf("REPOSITORY STATISTICS\n");
            for(int i=0;i<10;i++){ //// using a loop to show the statistics of the repository
                printf("Item %d = %d\n",i+1,*(repos+i));
            }
        }
        printf("\n\n");
        printf("Consuming:\n");
        rand_item=rand()%10; /// using a global varibale to choose a random  item
        rand_to_be_served=1+rand()%100; ////using a global varibale to choose a random quantity 
        if(rand_to_be_served<=repos[rand_item]){ /// conditon if the repository has enough requested quantity  
            printf("\nDemand has been Served; ITEM %d had %d quantities and demand was %d\n\n",rand_item+1,repos[rand_item],rand_to_be_served);
            repos[rand_item]-=rand_to_be_served; /// updating the repository only if the demand is satisfied
            printf("REPOSITORY STATISTICS\n");
            for(int i=0;i<10;i++){
                printf("Item %d = %d\n",i+1,*(repos+i));
            }
            printf("\n\n");
            pthread_mutex_unlock(&mutex); /// unlocking the mutex to let other threads
                                        //// use the global varibales
        }else{ /// if the repository has not enough requested quantity 
            ready--; /// decrement the indicator to let the producer know that it must produce
            pthread_mutex_unlock(&mutex);  //// unlock the resources to let the other
                                           ///// consumers use the global variables
            printf("Repository wasn't sufficient\n");
            printf("Item %d has %d quantities and demand is %d\n\n",rand_item+1,repos[rand_item],rand_to_be_served);
        }
        
    }
}

int main(int argc,char* argv[]){
    int res;
    int item[10]={0}; /// declaring an array of repository of
                      //// 10 items and initilizing it with 0 quantity
    int *rep; /// declaring an integer pointer to pass the items to the threads
    rep=item; /// pointer to the array
    if (argc!=2){ /// Checking the number of arguments
            fprintf(stderr, "Wrong Number of Arguments\n");
            exit(EXIT_FAILURE);
    }
    int n=atoi(argv[1]); /// cast the command line from char to integer
    srand(time(NULL)); /// using the system clock to set the seeds for the random function
    //////allocating memory for the thread IDs//////
    pthread_t* p_ths=malloc(n*sizeof(pthread_t));
    pthread_t* c_ths=malloc(n*sizeof(pthread_t));
    pthread_attr_t attr;
    pthread_attr_init(&attr); /// initializing the attributes of the threads
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
    printf("..........INITIALIZING..........\n\n");
    printf("REPOSITORY STATISTICS\n");
    for(int i=0;i<10;i++){
        printf("Item %d = %d\n",i+1,item[i]);
    }
    printf("\n");
    printf(".................................\n");
    /////creating n number of producer threads using a for loop
    /////with attributes that have chosen and the function
    ///// that has been already declared in the global space
    for(unsigned int i=0;i<n;i++){
        res=pthread_create(p_ths+i,&attr,producer,(void*) rep);
        if(res!=0){ // raising an error if the creation has failed
            fprintf(stderr, "Failed to create thread\n");
            exit(EXIT_FAILURE);
        }
    }
     /////creating n number of consumer threads using a for loop
    for(unsigned int i=0;i<n;i++){
        res=pthread_create(c_ths+i,&attr,consumer,(void*) rep);
        if(res!=0){// raising an error if the creation has failed
            fprintf(stderr, "Failed to create thread\n");
            exit(EXIT_FAILURE);
        }
    }
    for(unsigned int i=0;i<n;i++){
        ///// using this function here let the main thread wait
        ///// for the thread which is declared in the pthread_join to finish
        res=pthread_join(*(p_ths+i),NULL);
            if(res!=0){  // raising an error if the joining has failed
            fprintf(stderr, "Failed to join the thread\n");
            exit(EXIT_FAILURE);
        }
    }
    for(unsigned int i=0;i<n;i++){
        res=pthread_join(*(c_ths+i),NULL);
        if(res!=0){// raising an error if the joining has failed
            fprintf(stderr, "Failed to join the thread\n");
            exit(EXIT_FAILURE);
        }
    }
    ///// free the memory that was used for the thread IDs////
    free(p_ths);
    free(c_ths);
    pthread_attr_destroy(&attr); /// destroy the attribute at the end (Optional)
    return 0;
}