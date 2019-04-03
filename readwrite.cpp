#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>

struct Node {
	Node * next;
	std::string data;
	Node(std::string data){

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

	void insert(Node * node){
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

int main(int argc, char * argv[]){
	int n;
	int r;
	int w;

	if ( argc == 4 ){
		n = atoi(argv[1]);
		r = atoi(argv[2]);
		w = atoi(argv[3]);
		if ( ( (n >= 1) && (n <= 1000) ) && ( (r >= 1) && (r <= 9) ) && ( (w >= 1) && (w <= 9) ) ){
			//Operations for linked list go here. Gross, ik
			Node * n1 = new Node("10");
			Node * n2 = new Node("15");
			LinkedList * ll = new LinkedList(n1);
			ll->insert(n2);
			ll->traverse();
		}
		else {
			std::cout << "\nIncorrect parameters: readwrite N R W where 1 <= N <= 1000, 1 <= R <= 9, and 1 <= W <= 9\n" << std::endl;
			return 1;
		}
			
	}
	else {
		std::cout << "\nIncorrect parameters: readwrite N R W where 1 <= N <= 1000, 1 <= R <= 9, and 1 <= W <= 9\n" << std::endl;
		return 1;
	}
	return 0;
}
