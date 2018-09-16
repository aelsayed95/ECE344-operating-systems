#ifndef _ADDRSPACE_H_
#define _ADDRSPACE_H_

#include <vm.h>
#include "opt-dumbvm.h"

#define VM_STACKPAGES    24

struct vnode;


/* 
 * Address space - data structure associated with the virtual memory
 * space of a process.
 *
 * You write this.
 */

struct pte{
	paddr_t pa;
	vaddr_t va;
	short rwx;
	short on_mem;
	short on_disk;
	struct pte *next;
};

struct region_array{
	paddr_t pa;
	vaddr_t va;
	short rwx;
	size_t num_pages;
	struct region_array *next;
};

struct addrspace {
#if OPT_DUMBVM
	vaddr_t as_vbase1;
	paddr_t as_pbase1;
	size_t as_npages1;
	vaddr_t as_vbase2;
	paddr_t as_pbase2;
	size_t as_npages2;
	paddr_t as_stackpbase;
#else
	/* Put stuff here for your VM system */
	struct pte *pages;
	struct pte *last_page;
	
	struct pte *stack;
	struct pte *last_stack;
	vaddr_t stack_begin, stack_end;

	struct pte *heap;
	struct pte *last_heap;
	vaddr_t heap_begin, heap_end;

	struct region_array *regions;
	struct region_array *last_region;

#endif
};

/*
 * Functions in addrspace.c:
 *
 *    as_create - create a new empty address space. You need to make 
 *                sure this gets called in all the right places. You
 *                may find you want to change the argument list. May
 *                return NULL on out-of-memory error.
 *
 *    as_copy   - create a new address space that is an exact copy of
 *                an old one. Probably calls as_create to get a new
 *                empty address space and fill it in, but that's up to
 *                you.
 *
 *    as_activate - make the specified address space the one currently
 *                "seen" by the processor. Argument might be NULL, 
 *		  meaning "no particular address space".
 *
 *    as_destroy - dispose of an address space. You may need to change
 *                the way this works if implementing user-level threads.
 *
 *    as_define_region - set up a region of memory within the address
 *                space.
 *
 *    as_prepare_load - this is called before actually loading from an
 *                executable into the address space.
 *
 *    as_complete_load - this is called when loading from an executable
 *                is complete.
 *
 *    as_define_stack - set up the stack region in the address space.
 *                (Normally called *after* as_complete_load().) Hands
 *                back the initial stack pointer for the new process.
 */

struct addrspace *as_create(void);
//int               as_copy(struct addrspace *src, struct addrspace **ret);
int 				as_copy(volatile struct addrspace *old, volatile struct addrspace **ret);
void              as_activate(struct addrspace *);
void              as_destroy(struct addrspace *);

int               as_define_region(struct addrspace *as, 
				   vaddr_t vaddr, size_t sz,
				   int readable, 
				   int writeable,
				   int executable);
int		  as_prepare_load(struct addrspace *as);
int		  as_complete_load(struct addrspace *as);
int               as_define_stack(struct addrspace *as, vaddr_t *initstackptr);


// from vm.c
paddr_t demand_page(struct pte *entry, struct addrspace *as);
paddr_t load_page(struct pte *entry, struct addrspace *as, int faulttype);
unsigned long find_lru();
unsigned long get_space_on_disk(unsigned long index, vaddr_t faultaddress, swap_type_t swap_type);
void swap_out(unsigned long offset,vaddr_t va);
void swap_in(unsigned long offset, vaddr_t va);
struct pte * update_pte(unsigned long index);
void update_cmap(unsigned long index, struct addrspace *as, cmap_state_t state, page_state_t pstate);

/*
 * Functions in loadelf.c
 *    load_elf - load an ELF user program executable into the current
 *               address space. Returns the entry point (initial PC)
 *               in the space pointed to by ENTRYPOINT.
 */

int load_elf(struct vnode *v, vaddr_t *entrypoint);


#endif /* _ADDRSPACE_H_ */
