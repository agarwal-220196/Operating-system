#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

#define PAGE_DIRECTORY_FRAME_SIZE 1

#define PAGE_PRESENT 1 // because this is represented by the bit 0
#define PAGE_WRITE   2 // because page write option is represented by the bit 1
#define PAGE_LEVEL_USER 4 // bit 2 defines the user level or the kernel level

#define PAGE_DIRECT_ADDR	22// as per the frame of address. 10+10+12 (direc+table+info)
#define PAGE_TABLE_ADDR		12// as per the frame of address. 10+10+12 (direc+table+info)
#define EXCLUDE_LAST_12_BITS	0xFFFFF000 //excluding the last 12 bits of info/offset
#define PAGE_TABLE_MASK		0x3FF // used to get only the 10 bits will be used in page fault handler



/*
bit 0 - high-->page present; low-->page not present 
bit 1 - high-->read and write; low-->read only 
bit 2 - high-->user mode; low--> kernel mode
*/

/*

the enabling of paging bit is referred from K.J.'s tutorial on 
www.osdever.net/tutorials/view/implementing-basic-paging

*/

//initialization to null 
PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   //assert(false);
   PageTable::kernel_mem_pool = _kernel_mem_pool;//assiging the passed variables to static variables. 
   PageTable::process_mem_pool = _process_mem_pool;
   PageTable::shared_size = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
	//assert(false);
	

	page_directory = (unsigned long*)(process_mem_pool->get_frames(PAGE_DIRECTORY_FRAME_SIZE)*PAGE_SIZE);//this will assign the page directory space. CHANGING IT TO PROCESS MEMORY SPACE AS SPECIFIED IN MP4 

	unsigned long address_value = 0;// it will be used to set reset the lower bits of entries

	unsigned long * page_table_mapped_directly = (unsigned long *)(process_mem_pool->get_frames(PAGE_DIRECTORY_FRAME_SIZE)*PAGE_SIZE);//CHANGING IT TO PROCESS MEMORY SPACE AS SPECIFIED IN MP4

	unsigned long number_of_shared_frames = (PageTable::shared_size/PAGE_SIZE);//indicates the number of frames for shared memory spaces.


	/*
	As mentioned in the handout, marking the shared memory with 
	(USER LEVEL) kernel mode | read & write | present  
	*/
	//since the first 4mb is directly mapped, we can assign it directly
	for (int i = 0; i<number_of_shared_frames; i++){//number of shared frames will be 1024 since we aare sharing only the first 4MB 
		
		page_table_mapped_directly[i] = address_value | PAGE_WRITE | PAGE_PRESENT;// since it is a shared memory, we will have a write access and the page is present since it is a directly mapped memory. 
		address_value += PAGE_SIZE;
	

}//for i=0

	page_directory[0] = (unsigned long) page_table_mapped_directly | PAGE_WRITE |PAGE_PRESENT;//doing the or operation with the first page table and storing it in the page directory 0. Since it is mapped directly, it is available and it can be written as well. 

	address_value =0;
	for (int i=1; i<number_of_shared_frames;i++){
	page_directory[i]=address_value | PAGE_WRITE;// marking it non present since it is not available yet 
	
}//for i=1

	page_directory[number_of_shared_frames-1] = (unsigned long)(page_directory) | PAGE_WRITE | PAGE_PRESENT;//marking the last entry as present (recursive page table) 1023
	
	registered_vm_pool_count=0;//since when this constructor is called, we are not assiging any space, we are doing it in the register_pool function 	

	for (int i=0; i<MAX_VIRTUAL_MEMORY_POOLS;i++){
	registered_vm_pool[i] = NULL;//reason same as above for marking it will NULL 
}//for i=0

	Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
	//assert(false);
	current_page_table = this;
	write_cr3((unsigned long)page_directory);
	Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
	//assert(false);
	paging_enabled = 1;
	write_cr0(read_cr0() | 0x80000000);//this is referred from K.J tutorial on implementing basic paging as mentioned in the handout. 
	Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
	//assert(false);
	// as defined in the machine.H file, we define the error code in err_code variable. 
	
//we need to do recursive lookup to get the address
	//unsigned long * current_page_directory = (unsigned long *) read_cr3();//contains the page directory base address.
	unsigned long page_address = read_cr2();//contains the 32 bit address that casued the page fault 


/*
As defined in the X86 the addresses are as follows 

10 bits for page directory 	10 bits for pages 	12 bits info offset 
i.e.	0000 0000 00		00 0000	0000		0000 0000 0000 
*/

//EXCLUDE_LAST_12_BITS
//PAGE_TABLE_MASK
	

//separating the two address
 	unsigned long page_direct_addr = page_address >> PAGE_DIRECT_ADDR;
	unsigned long page_table_addr  = page_address >> PAGE_TABLE_ADDR;
		
	unsigned long * page_table = NULL;//will be used to store the page table entry 
	unsigned long error_code   = _r->err_code;// read the error code 

	unsigned long  user_level_mask = 0; //will be used to or the data with page_level


//recursive lookup

	unsigned long * current_page_directory = (unsigned long *) 0xFFFFF000; // default directory location as mentioned in slide 



	if ((error_code & PAGE_PRESENT)==0){


	int pool_index = -1;// will be used to access the pool of that page table. 
	
	VMPool ** virtualmemory_pool = current_page_table->registered_vm_pool;

	for (int i=0;i<current_page_table->registered_vm_pool_count;i++){
		
		if (virtualmemory_pool[i]!=NULL){//if there exists such a pool then only enter
			if (virtualmemory_pool[i]->is_legitimate(page_address)){//if the address belongs to that pool then only mark it as present. 
				pool_index=i;
				break;
}//second if 

}// if 

}//for 
	

	assert(!(pool_index<0));// check if the pool_index is positive or negative. 
//if the pool_index is negative then halt as the address that is a pagefault does not belong 
//to any of the pools. 



		if ((current_page_directory[page_direct_addr] & PAGE_PRESENT)==1){//indicates a missing page table entry
			
			//page_table = (unsigned long *)(current_page_directory[page_direct_addr]&EXCLUDE_LAST_12_BITS);//exlude the offset 12 bits 
		

			page_table = (unsigned long *)(0xFFC00000 | (page_direct_addr<< PAGE_TABLE_ADDR));

			page_table[page_table_addr & PAGE_TABLE_MASK] = (PageTable::process_mem_pool->get_frames(PAGE_DIRECTORY_FRAME_SIZE)*PAGE_SIZE) | PAGE_WRITE |PAGE_PRESENT;// request a new page from the process memory 
			
}//if  
		else {// inidiactes a missing page table i.e. missing page directory entry 
		
			current_page_directory[page_direct_addr] = (unsigned long)((process_mem_pool->get_frames(PAGE_DIRECTORY_FRAME_SIZE)*PAGE_SIZE) | PAGE_WRITE |  PAGE_PRESENT);//used to request a new PAGE DIRECTORY ENTRY 
			//page_table = (unsigned long *)(current_page_directory[page_direct_addr]&EXCLUDE_LAST_12_BITS);//get the page table of the missing one from the CR3 address. same as above 

			page_table = (unsigned long *)(0xFFC00000| (page_direct_addr<< PAGE_TABLE_ADDR));
		
                        for (int i=0;i<1024;i++){

				page_table[i]= user_level_mask | PAGE_LEVEL_USER;//fills the page table entries by default to user level pages. 
}//for i =0 to 1024
			page_table[page_table_addr & PAGE_TABLE_MASK] = (PageTable::process_mem_pool->get_frames(PAGE_DIRECTORY_FRAME_SIZE)*PAGE_SIZE) | PAGE_WRITE |PAGE_PRESENT;//used to request a new PAGE TABLE ENTRY at the address where fault occured. address taken from CR3
			


}//else of page directory 
}//if error code & present =1
	Console::puts("handled page fault\n");
}

void PageTable::register_pool(VMPool * _vm_pool)
{

	if (registered_vm_pool_count< MAX_VIRTUAL_MEMORY_POOLS){// check if number of pools less than the number of max pools allowed 

		registered_vm_pool[registered_vm_pool_count++]= _vm_pool;
		Console::puts("VM pool is registered \n");

}//if end 
	else{
		Console::puts("Virtual memory pools are full, cannot register \n");// if less number of pools then do not register. 
}//else end 

}

void PageTable::free_page(unsigned long _page_no){

	unsigned long page_direct_addr = _page_no >> PAGE_DIRECT_ADDR;// get the page directory 
        unsigned long page_table_addr  = _page_no >> PAGE_TABLE_ADDR;//get the page table address 
        unsigned long * page_table = (unsigned long *)(0xFFC00000 | (page_direct_addr << PAGE_TABLE_ADDR));//recursive page table location 

	unsigned long frame_number = page_table[page_table_addr & PAGE_TABLE_MASK] / (Machine::PAGE_SIZE);//get the frame number using the recursive logic 

	process_mem_pool->release_frames(frame_number);//call the release frame number 

	page_table[page_table_addr & PAGE_TABLE_MASK] = 0 | PAGE_WRITE;// mark the PTE empty. 


//FLUSH TLB 

	unsigned long * current_page_directory = (unsigned long *)read_cr3();
	write_cr3((unsigned long)current_page_directory); 

}


