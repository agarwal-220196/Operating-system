/*
     File        : blocking_disk.c

     Author      : Sanket Vinod Agarwal 
     Modified    : April, 15, 2020

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "scheduler.H"
#include "thread.H"

extern Scheduler *SYSTEM_SCHEDULER // will be used to call different functions of scheduler class
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
	disk_queue_size = 0; //initialize to zero 
	this->disk_queue = new round_robin_queue();

}

void BlockingDisk::wait_until_ready(){
	if (!SimpleDisk::is_ready()){//if not ready then yield the CPU 
		Thread *thread_current = Thread::CurrentThread();//get the current thread
		this->disk_enqueue(thread_current);//add to the queue
		SYSTEM_SCHEDULER->yield();// yield the cpu 
	}

}

void BlockingDisk::disk_enqueue(Thread *thread){
	this->disk_queue->enqueue_thread(thread);// do the queue addition task
	disk_queue_size++;// increase the size of queue
}

bool BlockingDisk::is_ready(){// this function will be called from the scheduler to check if the thread has completed its I/O. That is the periodic check. 
	return SimpleDisk::is_ready();
}
/*------------------------------------------------------------------------------
	SIMPLE_DISK FUNCTIONS not adding the functionalities here but modifying the wait until ready function. */
/*--------------------------------------------------------------------------*/

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  SimpleDisk::read(_block_no, _buf);

}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  SimpleDisk::write(_block_no, _buf);
}
