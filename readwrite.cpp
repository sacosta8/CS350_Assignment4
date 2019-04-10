#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<time.h>

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

	void traverse(){
		Node * temp = head;
		while(temp){
			std::cout << temp->data << std::endl;
			temp = temp->next;
		}
	}

	void insert(int data){
		Node * node = new Node(data);
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

struct readerArgs {
	int readerNumber;
	int n;
	LinkedList * list;
};

struct writerArgs {
	int writerNumber;
	int n;
	int data;
	LinkedList * list;
};

void * reader( void * arguments){
	readerArgs * args = (readerArgs *) arguments;
	std::string fileNumber = std::to_string(args->readerNumber);
	std::string filename = "reader_" + fileNumber + ".txt";
	std::fstream file;
	file.open(filename.c_str(),std::fstream::out);

	file << filename + "\n";

	file.close();
}

void * writer(void * arguments){
	writerArgs * args = (writerArgs *) arguments;
	args->list->insert(args->data);	

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
			LinkedList * list = (LinkedList *) malloc(sizeof(LinkedList));
			pthread_t readerThreads[r];
			pthread_t writerThreads[w];
			for(int i = 0; i < r; i++){
				readerArgs * args = (readerArgs *) malloc(sizeof(readerArgs));
				args->readerNumber = i+1;
				args->n = n;
				args->list = list;
				pthread_create(&readerThreads[i],NULL,reader, (void *) args);
			}
			for(int i = 0; i < w; i++){
				writerArgs * args = (writerArgs *) malloc(sizeof(writerArgs));
				args->writerNumber = i+1;
				args->n = n;
				args->list = list;
				//To make writer random number
				int randNum = rand() % 1000 + 1;
				int difference = (randNum % 10) - (i+1);
				if(difference > 0)
					randNum -= difference;
				if(difference < 0)
					randNum += difference * -1;
				args->data = randNum;
				pthread_create(&writerThreads[i],NULL,writer, (void *) args);
			}
			list->traverse();

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
