/*
 File: scheduler.C
 
 Author: Sanket Vinod Agarwal 
 Date  : Started on March 29, 2020
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
//  assert(false);
	size_of_queue = 0; // at the start initializing the queue size to be zero
	this->block_disk = NULL; // initialize the block disk pointer to null initially. 
	Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
//  assert(false);
	// give priority to the thread waiting in the disk_queue. if not that move to the queue of normal threads that are not waiting for I/O operations. 
	if( (block_disk!=NULL) && (block_disk->is_ready()) && (block_disk->disk_queue_size != 0) ){//if disk_queue has a thread then process that 
		Thread *top_thread_disk_queue = block_disk->disk_queue->dequeue();//take the thread from queue
		block_disk->disk_queue_size--; //reduce the size of the queue
		Thread::dispatch_to(top_thread_disk_queue);// send the thread for execution. 
	
	}else{//no thread in the disk queue
		if (size_of_queue==0){
		// that is no other thread is available to be executed. 
			Console::puts("No other thread is available and the queue is empty \n");
		}else {
			size_of_queue--;// reduce the queue size by 1 since we will be removing one of the thread form the queue and placing it in the CPU

			Thread* new_thread = ready_queue.dequeue();// that is remove the top of queue thread and place it in the new_thread variable. 
		
	// now load this new thread obtained from queue into the cPU by calling the dispatch function 
			Thread::dispatch_to (new_thread);
		}
	}
}

void Scheduler::resume(Thread * _thread) {
  //assert(false);
	ready_queue.enqueue_thread(_thread);// add the thread obtained in the argument to the queue.
	size_of_queue++;
}

void Scheduler::add(Thread * _thread) {
  //assert(false);

	ready_queue.enqueue_thread(_thread);
	size_of_queue++; //same function as the ready queue. 
}

void Scheduler::terminate(Thread * _thread) {
//  assert(false);
// the idea is to check all the threads in the queue with the thread to be terminated. if a matching thread is found, then it is not added back and the queue size is reduced. If it is not matched then it adds back to the queue. 
	for (int i=0; i<size_of_queue;i++){
		Thread * top_thread = ready_queue.dequeue();
		if (top_thread->ThreadId() == _thread->ThreadId()){
			size_of_queue--;
		}else{
			ready_queue.enqueue_thread(top_thread);
		}
	}
}

void Scheduler::pass_kernel_disk_object_to_scheduler (BlockingDisk *disk_object){
	block_disk = disk_object; // pass the object received from kernel to the object used for calling all the methods. 
}
