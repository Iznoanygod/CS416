#include "my_vm.h"

/*
Function responsible for allocating and setting your physical memory
*/
int physical_pages = MEMSIZE/PGSIZE;
int virtual_pages = MAX_MEMSIZE/PGSIZE;
static char* ppage_states;
static char* vpage_states;
static char* physical_memory;

static pde_t* pagedir[1024][1024];

static int nextp = -1;

int bitMask(int n, int k, int p){
    return (((1 << k) -1) & (n >> (p-1)));
}

void SetPhysicalMem() {

    //Allocate physical memory using mmap or malloc; this is the total size of
    //your memory you are simulating


    //HINT: Also calculate the number of physical and virtual pages and allocate
    //virtual and physical bitmaps and initialize them
    
    physical_memory = malloc(MEMSIZE);
    ppage_states = malloc(physical_pages);
    vpage_states = malloc(virtual_pages);
    for(int i = 0; i < physical_pages; i++){
        ppage_states[i] = 0;
    }
    for(int i = 0; i < virtual_pages; i++){
        vpage_states[i] = 0;
    }

}


/*
 * Part 2: Add a virtual to physical page translation to the TLB.
 * Feel free to extend the function arguments or return type.
 */
int add_TLB(void *va, void *pa)
{

    /*Part 2 HINT: Add a virtual to physical page translation to the TLB */

    return -1;
}


/*
 * Part 2: Check TLB for a valid translation.
 * Returns the physical page address.
 * Feel free to extend this function and change the return type.
 */
pte_t * check_TLB(void *va) {

    /* Part 2: TLB lookup code here */

}


/*
 * Part 2: Print TLB miss rate.
 * Feel free to extend the function arguments or return type.
 */
void print_TLB_missrate()
{
    double miss_rate = 0;

    /*Part 2 Code here to calculate and print the TLB miss rate*/




    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}


/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
pte_t * Translate(pde_t *pgdir, void *va) {
    //HINT: Get the Page directory index (1st level) Then get the
    //2nd-level-page table index using the virtual address.  Using the page
    //directory index and page table index get the physical address
    
    uintptr_t addr = (uintptr_t) va;
    int i = bitMask(addr, 10, 23);
    int j = bitMask(addr, 10, 13);
    pde_t* retv = pagedir[i][j];
    return retv;

    //If translation not successfull
    return NULL;
}


/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int PageMap(pde_t *pgdir, void *va, void *pa)
{

    /*HINT: Similar to Translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */
    uintptr_t i = (uintptr_t)pgdir;
    uintptr_t j = (uintptr_t)va;
    if(pagedir[i][j] == NULL)
        pagedir[i][j] = pa;
    else{
        printf("Mapping Already Exists\n");
        return 0;
    }
    return -1;
}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {

    //Use virtual address bitmap to find the next free page

    nextp = -1;
    for(int i = 0; i < virtual_pages; i++){
        if(!vpage_states[i]){
            nextp = i;
            return NULL;
        }
    }
    printf("No more available pages\n");
    exit(1);
}


/* Function responsible for allocating pages
and used by the benchmark
*/
void *myalloc(unsigned int num_bytes) {

    //HINT: If the physical memory is not yet initialized, then allocate and initialize.
    

    if(physical_memory == NULL){
        SetPhysicalMem();
    }
    get_next_avail(virtual_pages);
    int pages_needed = num_bytes / PGSIZE + 1;
    if(nextp == -1){
        printf("Can not find next page\n");
        exit(1);
    }
    for(int i = nextp, j = 0; j < pages_needed; i++,j++){
        vpage_states[i] = 1;
    }
    uintptr_t addr = (unsigned int)physical_memory + PGSIZE *nextp;
    
    int i = bitMask(addr, 10, 23);
    int j = bitMask(addr, 10, 13);
    
    if(pagedir[i][j] == NULL){
        pagedir[i][j] = (unsigned long int*) addr;
    }
    else{
        printf("Mapping already exists, maybe a prior mapping");
    }
    return (void*) addr;
   /* HINT: If the page directory is not initialized, then initialize the
   page directory. Next, using get_next_avail(), check if there are free pages. If
   free pages are available, set the bitmaps and map a new page. Note, you will
   have to mark which physical pages are used. */

    return NULL;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void myfree(void *va, int size) {

    //Free the page table entries starting from this virtual address (va)
    // Also mark the pages free in the bitmap
    //Only free if the memory from "va" to va+size is valid
    pte_t* paddr = Translate(NULL, va);

    int frees = ((int)paddr - (unsigned int)physical_memory)/PGSIZE;
    int nfrees = size / PGSIZE + 1;
    for(int i = frees, j = 0; j < nfrees; i++,j++){
        vpage_states[frees] = 0;
    }

    int addr = (unsigned long int)paddr;
    int i = bitMask(addr, 10, 23);
    int j = bitMask(addr, 10, 13);
    pagedir[i][j] = 0;
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
*/
void PutVal(void *va, void *val, int size) {

    /* HINT: Using the virtual address and Translate(), find the physical page. Copy
       the contents of "val" to a physical page. NOTE: The "size" value can be larger
       than one page. Therefore, you may have to find multiple pages using Translate()
       function.*/
    pte_t* paddr = Translate(NULL, va);
    memcpy(paddr, val, size);
    /*for(int i = 0; i < (size > MEMSIZE ? (size/MEMSIZE):1); i++){
        pte_t* paddr = Translate(NULL, va+i);
        memcpy(paddr,val+i, MEMSIZE);
    }*/

}


/*Given a virtual address, this function copies the contents of the page to val*/
void GetVal(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    "val" address. Assume you can access "val" directly by derefencing them.
    If you are implementing TLB,  always check first the presence of translation
    in TLB before proceeding forward */
    pte_t* paddr = Translate(NULL, va);
    memcpy(val, paddr, size);
    /*for(int i = 0; i < (size > MEMSIZE ? (size / MEMSIZE):1); i++){
        pte_t* paddr = Translate(NULL, va+i);
        memcpy(val+i, paddr, MEMSIZE);
    }*/

}



/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void MatMult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
    matrix accessed. Similar to the code in test.c, you will use GetVal() to
    load each element and perform multiplication. Take a look at test.c! In addition to
    getting the values from two matrices, you will perform multiplication and
    store the result to the "answer array"*/
    int* M = malloc(size * size * sizeof(int));
    int* N = malloc(size * size * sizeof(int));
    int* RES = malloc(size * size * sizeof(int));
    int x,y=0;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            int n1 = (unsigned int)mat1 + ((i * size * sizeof(int))) + (j * sizeof(int));
            int n2 = (unsigned int)mat2 + ((i * size * sizeof(int))) + (j * sizeof(int));
            GetVal((void*)n1, &x, sizeof(int));
            GetVal((void*)n2, &y, sizeof(int));
            M[size*i+j]=x;
            N[size*i+j]=y;
        }
    }
    for(int i = 0; i < size; i++){
        for(int j =0; j < size; j++){
            for(int k = 0; k < size; k++){
                int n1 = M[i*size+k];
                int n2 = N[k*size+j];
                RES[i*size+j] += n1*n2;
            }
        }
    }
    printf("didnt break yet\n");
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            int n1 = (unsigned int) answer + ((i * size * sizeof(int))) + (j * sizeof(int));
            PutVal((void*)n1, (void*)&RES[i*size+j], sizeof(int));
        }
    }

}
