#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <array>
#include <unistd.h>

using namespace std;

sem_t semSmoker[3]; //semaphore for smokers
sem_t semVendor; //vendors semaphore
int smokeCount=-1;
int semWait[3]; //count for the number of cigarettes smoked done by each smokers 
bool checkSmoked=false; //check smoker done
sem_t vendorWake; //to indicate when vendor is wake to complete next turn
sem_t smokerWake; //to indicate vendor that the smoker is waiting for vendor to give the items
int smokerCount=1;

string vendorsSets[]={"Tabacoo and Paper", "Paper and Matches", "Matches and Tabacoo"};

void *cigarette(void *pVoid){

    int *semNumber= (int*)(pVoid);
    
    for (int i = 1; i <=3; ++i){
        usleep( rand() % 10000 );
        cout<<"Smoker "<<*semNumber<<" need "<<vendorsSets[*semNumber-1]<<" to create smoke "<<endl;
        smokerCount++;
        if ( smokerCount>3 ){
            sem_post(&smokerWake);
        }
        sem_wait(&semSmoker[(*semNumber-1)]);
        if ( smokeCount == 0 ){
            usleep( rand() % 10000 );
            cout<<"\033[0;35m==> \033[0;33mSmoker1 is creating a cigarette\033[0;0m"<<endl;
    	    sleep(01);
	        cout<<"\033[0;31mSmoker1 is Smoking\033[0m \n";
            semWait[0]--;
            sem_post(&semVendor);
            cout<<"\033[0;35m==> \033[0;33mSmoker1 done smoking\033[0;0m"<<endl;
        }
        else  if ( smokeCount == 1 ){
            usleep(rand()%10000);
            cout<<"\033[0;35m==> \033[0;33mSmoker2 is creating a cigarette\033[0;0m"<<endl;
    		sleep(1);
	        cout<<"\033[0;31mSmoker2 is Smoking\033[0m \n";
	        semWait[1]--;
            sem_post(&semVendor);
            cout<<"\033[0;35m==> \033[0;33mSmoker2 done finish smoking\033[0;0m"<<endl;
        }
        else if ( smokeCount == 2 ){
            usleep(rand()%50000);
            cout<<"\033[0;35m==> \033[0;33mSmoker3 is creating a cigarette\033[0;0m"<<endl;
    		sleep(1);
	        cout<<"\033[0;31mSmoker3 is Smoking\033[0m \n";
	        semWait[2]--;
            sem_post(&semVendor);
            cout<<"\033[0;35m==> \033[0;33mSmoker3 done finish smoking\033[0;0m"<<endl;
        }
    }
    pthread_exit(NULL);
}

void *vendor(void *pVoid){
    srand(time(NULL));
    int a;
    a = *((int *) pVoid);

    for (int i = 0; i < 9; ++i){
    	sem_wait(&semVendor);

        while(true){
            int rndNum=(rand()%3);
            usleep(rand()%4000);
            
	    if( semWait[0]==0 && semWait[1]==0 && semWait[2]==0){
	        checkSmoked= true;
                break;
            }
            else if(semWait[rndNum]!=0){
                sleep(1);
                cout<<endl;
	        	cout<<"Vendor "<<a<<" put "<<vendorsSets[rndNum]<<" on table."<<endl<<endl;
                smokeCount=rndNum;
                sem_post(&semSmoker[rndNum]);
                break;
            }	
	}
    }
    return 0;
}

void * func(void *pVoid){
    sem_wait( &smokerWake );
    sem_post( &vendorWake );
    return 0;
}

int main(void){
    for (int i = 0; i < 3; ++i){
        semWait[i]=3;
    }
	
    sem_init(&semSmoker[0],0,0);
    sem_init(&semSmoker[1],0,0);
    sem_init(&semSmoker[2],0,0);
    sem_init(&semVendor,0,1);
    sem_init(&vendorWake,0,0);
    sem_init(&smokerWake,0,0);
   
    pthread_t smokerA,smokerB,smokerC,vendorA,pusher;
	
    int * at = new int[1];
    at[0]=1;
    int * at1 = new int[1];
    at1[0]=2;
    int * at2 = new int[1];
    at2[0]=3;
    
    
    pthread_create(&smokerA,NULL,cigarette,at );
    pthread_create(&smokerB,NULL,cigarette,at1);
    pthread_create(&smokerC,NULL,cigarette,at2);
    pthread_create(&vendorA,NULL,vendor,at);
    pthread_create(&pusher,NULL,func,NULL);
    
    pthread_join(smokerA,NULL);
    pthread_join(smokerB,NULL);
    pthread_join(smokerC,NULL);
	
    return 0;
}