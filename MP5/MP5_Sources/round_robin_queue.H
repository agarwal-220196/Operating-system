/*
File: round_robin_queue.H

Author: Sanket Vinod Agarwal

Date: Started on March 29,2020
*/

#ifndef QUEUE_H
#define QUEUE_H

#include "thread.H"

class round_robin_queue{
	private: 
		Thread* 		thread;//this will be pointing to the top of the queue
		round_robin_queue* 	next;// this will be pointing to the next thread in queue. 

// I have implemented this queue in linked list fashion there thread represents the head of the linked    list and next point to the following node in linked list. 
		
	public: 
		round_robin_queue(){

// this is the constructor that will be invoked when a handle of this class is created and initially all will be set to null. 
		
			thread = NULL;
			next   = NULL;
		}

		round_robin_queue(Thread * thread_new){
//this will be called multiple times whenever a new thread wants to enter the queue. A new node will be created with thread as the argument passed and next value will be null since it will be inserted at the end. 
			thread = thread_new;
			next = NULL;
		}
// Function to Enqueue the thread at the end

	void enqueue_thread (Thread * thread_new){

		if (thread==NULL){
		// it enters here if no thread has been in the queue and this is the first thread being entered. 
			thread = thread_new;
		} else {
			if (next==NULL ){// that is we have reached the end of linked list 

				next =  new round_robin_queue(thread_new);// create a new node at the end of linkedlist 
			} else {
				// traverse to throught the linked list to reach the end of list
				next->enqueue_thread(thread_new);
				
			}
		}
	}

	Thread *dequeue(){// this will dequee the thread at the top and move the head of queue to the next thread, 
	
		if (thread==NULL)
			return NULL; 

		Thread * top_of_thread = thread;// get the first thread in the queue
		if (next !=NULL){
		// that is if there still threads pending in the queue to be executed upadte the queue

		thread 				= next->thread;// get the next thread in queue as the head 
		round_robin_queue * delete_node = next; // current node to be deleted. 
		next 				= next->next // get the pointer of third node which becomes the second node later. 
		delete delete_node;

		} else {
			// if no threads pending in the queue. 
			thread = NULL; 
		}

		return top_of_thread;
	}
};

#endif

