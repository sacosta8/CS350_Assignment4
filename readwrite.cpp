#include<iostream>
#include<unistd.h>
#include<fstream>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include<semaphore.h>
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

int global_reader = 0;
struct Node {
	Node * next;
	int data;
	Node(int data){
	this->next = NULL;
	this->data = data;
	}
};

class LinkedList {
public:
	Node * head;

	LinkedList(){
		this->head = NULL;
	}

	LinkedList(Node * head){
		this->head = head;
	}

	int traverse(int search){
		Node * temp = head;
		int count = 0;
		while(temp){
			if( ( (temp->data) % 10) == search)
				count++;
			temp = temp->next;
		}
		return count;
	}

	void insert(int data){
		Node * node = new Node(data);
		std::cout << data << std::endl;
		if(head == NULL){
			head = node;
			return;
		}
		Node * temp = head;
		while(temp->next)
			temp = temp->next;
		temp->next = node;
	}


};

struct locksAndCounts {
	int readcount;
	int writecount;
	sem_t rmutex; 
	sem_t wmutex;
	sem_t readTry;
	sem_t resource;
	locksAndCounts(){
		readcount = 0;
		writecount = 0;
		sem_init(&rmutex,0,1);
		sem_init(&wmutex,0,1);
		sem_init(&readTry,0,1);
		sem_init(&resource,0,1);
	}
};

struct readerArgs {
	int readerNumber;
	int n;
	LinkedList * list;
	locksAndCounts * lac;
	readerArgs(int i,int n){
		readerNumber = i;
		this->n = n;
		lac = NULL;
		list = NULL;
	}
};

struct writerArgs {
	int writerNumber;
	int n;
	LinkedList * list;
	locksAndCounts * lac;
	writerArgs(int i,int n){
		writerNumber = i;
		this->n = n;
		lac = NULL;
		list = NULL;
	}
};


void * reader( void * arguments){
	readerArgs * args = (readerArgs *) arguments;
	locksAndCounts * lac = args->lac;
	std::string fileNumber = std::to_string(args->readerNumber);
	std::string filename = "reader_" + fileNumber + ".txt";
	std::fstream file;
	file.open(filename.c_str(),std::fstream::out);
	for(int i = 0; i < args->n; i++){		
		sem_wait(&lac->readTry);	
			sem_wait(&lac->rmutex);
			lac->readcount++;
			if(lac->readcount == 1)
				sem_wait(&lac->resource);
			sem_post(&lac->rmutex);
		sem_post(&lac->readTry);
		int count = args->list->traverse(args->readerNumber);
		std::string message = "Reader " + std::to_string(args->readerNumber) + ": Read " + std::to_string(i) + ": " + std::to_string(count) + " values ending in " + std::to_string(args->readerNumber) + "\n";
		file << message;
		sem_wait(&lac->rmutex);
			lac->readcount--;
			if (lac->readcount == 0)
				sem_post(&lac->resource);
		sem_post(&lac->rmutex);
		std::cout << "reader" << args->readerNumber << " done" << std::endl;
    pthread_mutex_lock(&m);
    global_reader--;
    if (global_reader == 1) {
        pthread_cond_signal(&c);    
    }
    pthread_mutex_unlock(&m);
  
		struct timespec req = {0};
		int mil = rand() % 100 + 1;
		req.tv_nsec = mil * 1000000L;
		req.tv_sec = 0;
		nanosleep(&req,(struct timespec *) NULL);
	}

	file.close();

}

void * writer(void * arguments){
	writerArgs * args = (writerArgs *) arguments;
	locksAndCounts * lac = args->lac;
	for(int i = 0; i < args->n; i++){
		//To make writer random number
		int randNum = rand() % 1000 + 1;
		int difference = (randNum % 10) - args->writerNumber;
		if(difference > 0)
			randNum -= difference;
		if(difference < 0)
			randNum += difference * -1;
		sem_wait(&lac->wmutex);
			lac->writecount++;
			if (lac->writecount == 1)
				sem_wait(&lac->readTry);
		sem_post(&lac->wmutex);
		sem_wait(&lac->resource);
			args->list->insert(randNum);
		sem_post(&lac->resource);
		sem_wait(&lac->wmutex);
			lac->writecount--;
			if (lac->writecount == 0){
				sem_post(&lac->readTry);
			}
		sem_post(&lac->wmutex);
		struct timespec req = {0};
		req.tv_sec = 0;
		int mil = rand() % 100 + 1;
		req.tv_nsec = mil * 1000000L;
		nanosleep(&req,(struct timespec *) NULL);
	}
	std::cout << "Writer " << args->writerNumber << " done" << std::endl;
}

void * readerCount(void * args) {
      pthread_mutex_lock(&m);
      while (global_reader != 1) 
        pthread_cond_wait(&c, &m);
      std::cout << "Almost Done!" << std::endl;
      pthread_mutex_unlock(&m);
}

int main(int argc, char * argv[]){
	int n;
	int r;
	int w;
	srand(time(NULL));

	if ( argc == 4 ){
		n = atoi(argv[1]);
		r = atoi(argv[2]);
		w = atoi(argv[3]);
		if ( ( (n >= 1) && (n <= 100) ) && ( (r >= 1) && (r <= 9) ) && ( (w >= 1) && (w <= 9) ) ){
      global_reader = r*n;
			LinkedList * list = new LinkedList();
			locksAndCounts * lac = new locksAndCounts();
			pthread_t readerThreads[r];
			pthread_t writerThreads[w];
      pthread_t cond_thread;
			for(int i = 0; i < r; i++){
				readerThreads[i] = i;
				readerArgs * args = new readerArgs(i,n);
				args->readerNumber = i+1;
				args->n = n;
				args->list = list;
				args->lac = lac;
				pthread_create(&readerThreads[i],NULL,reader, (void *) args);
			}
      pthread_create(&cond_thread,NULL, readerCount, NULL);  
			for(int i = 0; i < w; i++){
				writerThreads[i] = i;
				writerArgs * args = new writerArgs(i,n);
				args->writerNumber = i+1;
				args->n = n;
				args->list = list;
				args->lac = lac;
				pthread_create(&writerThreads[i],NULL,writer, (void *) args);
			}
			for(int i = 0; i < w; i++){
				pthread_join(writerThreads[i],NULL);
			}
			
			for(int i = 0; i < r; i++)
				pthread_join(readerThreads[i],NULL);
      
      pthread_join(cond_thread, NULL);

		}
		else {
			std::cout << "\nIncorrect parameters: readwrite N R W where 1 <= N <= 100, 1 <= R <= 9, and 1 <= W <= 9\n" << std::endl;
			return 1;
		}
			
	}
	else {
		std::cout << "\nIncorrect parameters: readwrite N R W where 1 <= N <= 100, 1 <= R <= 9, and 1 <= W <= 9\n" << std::endl;
		return 1;
	}
	return 0;
}
