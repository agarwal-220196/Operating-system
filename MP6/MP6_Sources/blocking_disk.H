/*
     File        : blocking_disk.H

     Author      :  Sanket Vinod Agarwal 

     Date        : April 15, 2020
     Description : 

*/

#ifndef _BLOCKING_DISK_H_
#define _BLOCKING_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"
#include "thread.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* B l o c k i n g D i s k  */
/*--------------------------------------------------------------------------*/

class round_robin_queue; // forward class declaration. 

class BlockingDisk : public SimpleDisk {


protected:
	void wait_until_ready();// will be used to check if ready or not and add to the disk_queue
	void disk_enqueue (Thread *thread);// will be used to actually add to the queue. 


public:

	int disk_queue_size;// size of the disk_queue
	round_robin_queue *disk_queue;// actual disk_queue different from the normal thread queue. 

   BlockingDisk(DISK_ID _disk_id, unsigned int _size); 
   /* Creates a BlockingDisk device with the given size connected to the 
      MASTER or SLAVE slot of the primary ATA controller.
      NOTE: We are passing the _size argument out of laziness. 
      In a real system, we would infer this information from the 
      disk controller. */

   /* DISK OPERATIONS */

   virtual void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

	bool is_ready(); // will be used to call the is ready of simple disk. 
};

#endif
