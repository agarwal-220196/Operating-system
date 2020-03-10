/*
 File: vm_pool.C
 
 Author:Sanket Vinod Agarwal 
 Date  :03/10/2020
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
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
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
	base_address = _base_address;
	size = _size;
	frame_pool = _frame_pool;
	page_table = _page_table;
	
	region_number = 0;//initalize it with zero 

	allocate_region = (struct allocate_region_ *)(base_address);//get the base address

	page_table->register_pool(this);//register the pool number region implemented in pagetable.C

	Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
    //assert(false);

	unsigned long address;

	if (size==0){
		
		Console::puts("Allocation of size 0 is invalid");
		return 0;	
}//if size ==1

	assert (region_number< MAX_NUMBER_REGIONS);//check if the number of regions allocated is less than the number of max regions. 

	unsigned long number_of_frames_required = _size / (Machine::PAGE_SIZE);//total number of frames required for the size requested. 

	unsigned long out_of_frame_size = _size % (Machine::PAGE_SIZE);//check if the requested size if in line with the frame size or not  

	if (out_of_frame_size > 0){
	
		number_of_frames_required ++ ;//additional frame required if the size is not a multiple of frame number 
}//out_of_frame_size =  0

	if (region_number==0){

//since the first page is used to maintain the information about allocated region, we start the allocation from next page number. 

		address = base_address;
		
		allocate_region[region_number].base_address = address + Machine::PAGE_SIZE;
		allocate_region[region_number].size = number_of_frames_required*(Machine::PAGE_SIZE);

		region_number++;
		Console::puts("Allocated region of memory.\n");
		return (address + Machine::PAGE_SIZE);

}//if region_number!=0 
	else {//if any previous region is allocated then assign the new region after the end of previous region 

		address = allocate_region[region_number-1].base_address + allocate_region[region_number-1].size;

}//end of else for region_number!=0	

	allocate_region[region_number].base_address = address;

	allocate_region[region_number].size = number_of_frames_required*(Machine::PAGE_SIZE);

	region_number++;
    	Console::puts("Allocated region of memory.\n");

	return address;
}

void VMPool::release(unsigned long _start_address) {
  //  assert(false);
	int current_region_number = -1;

//detecting the current region number 
	for (int i=0; i<MAX_NUMBER_REGIONS;i++){

		if (allocate_region[i].base_address==_start_address){
	
			current_region_number = i;
			break;
}

}//end of for loop that detects the region number 
	assert (!(current_region_number<0));//check if the region number is not less than zero


	unsigned int allocated_pages = ((allocate_region[current_region_number].size) / (Machine::PAGE_SIZE));


	for (int i = 0; i<allocated_pages;i++ ){//this for loop will free all the pages in the current_region_number.

		page_table->free_page(_start_address);
		_start_address += Machine::PAGE_SIZE;
}//allocated pages over



//if the region pages are removed, we need to reduce the number of regions allocated and move the lower region number to upper one. 

	for (int i = current_region_number;i<region_number-1;i++){

		allocate_region[i] = allocate_region[i+1];
}// end of regions 

	region_number--;

	page_table->load();

   	 Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
    assert(false);
    Console::puts("Checked whether address is part of an allocated region.\n");
}

