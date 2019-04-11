#include<iostream>
#include<unistd.h>
#include<fstream>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include<semaphore.h>

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
			int value;
			sem_getvalue(&lac->rmutex,&value);
			std::cout << "rmutex: " << value;
			if(lac->readcount == 1)
				sem_wait(&lac->resource);
			sem_post(&lac->rmutex);
		sem_post(&lac->readTry);
		int count = args->list->traverse(args->readerNumber);
		std::string message = "Reader " + std::to_string(args->readerNumber) + ": Read " + std::to_string(i) + ": " + std::to_string(count) + " values ending in " + std::to_string(args->readerNumber);
		file << message;
		sem_wait(&lac->rmutex);
		lac->readcount--;
		if (lac->readcount == 0)
			sem_post(&lac->resource);
		sem_post(&lac->rmutex);
		nanosleep(0,NULL);
	}

	file.close();

}

void * writer(void * arguments){
	writerArgs * args = (writerArgs *) arguments;
	locksAndCounts * lac = args->lac;
	for(int i = 0; i < args->n; i++){
		//To make writer random number
		int randNum = rand() % 1000 + 1;
		int difference = (randNum % 10) - (args->writerNumber+1);
		if(difference > 0)
			randNum -= difference;
		if(difference < 0)
			randNum += difference * -1;
		if (randNum > 999)
			std::cout << "osafhosdaifhsdaiofsaiodhfoisdahfiosdahfiosdahfiodahfoisdahoisdahfiodsahfosidahfiosdhfiosdahfoisdahfioadhfiosdhfioasdhfiosdafiodhsfoisdfio" << std::endl;
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
		if (lac->writecount == 0)
			sem_wait(&lac->readTry);
		sem_post(&lac->wmutex);
		nanosleep(0,NULL);
	}
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
			LinkedList * list = new LinkedList();
			locksAndCounts * lac = new locksAndCounts();
			pthread_t readerThreads[r];
			pthread_t writerThreads[w];
			for(int i = 0; i < w; i++){
				writerThreads[i] = i;
				writerArgs * args = new writerArgs(i,n);
				args->writerNumber = i+1;
				args->n = n;
				args->list = list;
				args->lac = lac;
				pthread_create(&writerThreads[i],NULL,writer, (void *) args);
			}
			for(int i = 0; i < r; i++){
				readerThreads[i] = i;
				readerArgs * args = new readerArgs(i,n);
				args->readerNumber = i+1;
				args->n = n;
				args->list = list;
				args->lac = lac;
				pthread_create(&readerThreads[i],NULL,reader, (void *) args);
			}
			for(int i = 0; i < w; i++)
				std::cout << pthread_join(writerThreads[i],NULL) << std::endl;
			
			for(int i = 0; i < r; i++)
				pthread_join(readerThreads[i],NULL);

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
