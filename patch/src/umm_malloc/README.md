[![verifier](https://github.com/rhempel/umm_malloc/actions/workflows/verifier.yml/badge.svg?branch=master&event=workflow_dispatch)](https://github.com/rhempel/umm_malloc/actions/workflows/verifier.yml)

# umm_malloc - Memory Manager For Small(ish) Microprocessors

This is a memory management library specifically designed to work with the
ARM7 embedded processor, but it should work on many other 32 bit processors,
as well as 16 and 8 bit devices.

You can even use it on a bigger project where a single process might want
to manage a large number of smaller objects, and using the system heap
might get expensive.

## Acknowledgements

Joerg Wunsch and the avr-libc provided the first `malloc()` implementation
that I examined in detail.

`http://www.nongnu.org/avr-libc`

Doug Lea's paper on malloc() was another excellent reference and provides
a lot of detail on advanced memory management techniques such as binning.

`http://gee.cs.oswego.edu/dl/html/malloc.html`

Bill Dittman provided excellent suggestions, including macros to support
using these functions in critical sections, and for optimizing `realloc()`
further by checking to see if the previous block was free and could be 
used for the new block size. This can help to reduce heap fragmentation
significantly. 

Yaniv Ankin suggested that a way to dump the current heap condition
might be useful. I combined this with an idea from plarroy to also
allow checking a free pointer to make sure it's valid.

Dimitry Frank contributed many helpful additions to make things more
robust including a user specified config file and a method of testing
the integrity of the data structures.

GitHub user @devyte provided useful feedback on the nesting of functions
as well as a fix for the problem that separates out the core free and
malloc functionality.

GitHub users @d-a-v and @devyte provided great input on establishing
a heap fragmentation metric which they graciously allowed to be used
in umm_malloc.

Katherine Whitlock (@stellar-aria) extended the library for usage in 
scenarios where more than one heap or memory space is needed.

## Usage

This library is designed to be included in your application as a
submodule that has default configuration that can be overridden
as needed by your application code.

The `umm_malloc` library can be initialized two ways. The first is
at link time:

- Set `UMM_MALLOC_CFG_HEAP_ADDR` to the symbol representing
  the starting address of the heap. The heap must be
  aligned on the natural boundary size of the processor.
- Set `UMM_MALLOC_CFG_HEAP_SIZE` to the size of the heap in bytes.
  The heap size must be a multiple of the natural boundary size of
  the processor.

This is how the `umm_init()` call handles initializing the heap.

We can also call `umm_init_heap(void *pheap, size_t size)` where the
heap details are passed in manually. This is useful in systems where
you can allocate a block of memory at run time - for example in Rust.

### Multiple heaps

For usage in a scenario that requires multiple heaps, the heap type
`umm_heap` is exposed. All API functions (`malloc`, `free`, `realloc`, etc.) 
have a corresponding `umm_multi_*` variant that take a pointer to this 
type as their first parameter.

Much like standard initialization, there are two methods:
   - `umm_multi_init(umm_heap *heap)`, which initializes a given heap
    using linker symbols
   - `umm_multi_init_heap(umm_heap *heap, void *ptr, size_t size)`, which
    will initialize a given heap using a known address and size.

## Automated Testing

`umm_malloc` is designed to be testable in standalone
mode using `ceedling`. To run the test suite, just make sure you have
`ceedling` installed and then run:

```
ceedling clean
ceedling test:all
```

## Configuration

> :warning: **You MUST provide a file called `umm_malloc_cfgport.h`
> somewhere in your app, even if it's blank**

The reason for this is the way the configuration override heirarchy
works. The priority for configuration overrides is as follows:

1. Command line defines using `-D UMM_xxx`
2. A custom config filename using `-D UMM_CFGFILE="<filename.cfg>"`
3. The default config filename `umm_malloc_cfgport.h`
4. The default configuration in `src/umm_malloc_cfg.h`


The following `#define`s are set to useful defaults in
`src/umm_malloc_cfg.h` and can be overridden as needed.

The fit algorithm is defined as either:

- `UMM_BEST_FIT` which scans the entire free list and looks
   for either an exact fit or the smallest block that will
   satisfy the request. This is the default fit method.
- `UMM_FIRST_FIT` which scans the entire free list and looks
   for the first block that satisfies the request.

The following `#define`s are disabled by default and should
remain disabled for production use. They are helpful when 
testing allocation errors (which are normally due to bugs in
the application code) or for running the test suite when
making changes to the code.

- `UMM_INFO` is used to include code that allows dumping
  the entire heap structure (helpful when there's a problem).

- `UMM_INTEGRITY_CHECK` is used to include code that
  performs an integrity check on the heap structure. It's
  up to you to call the `umm_integrity_check()` function.

- `UMM_POISON_CHECK` is used to include code that
  adds some bytes around the memory being allocated that
  are filled with known data. If the data is not intact
  when the block is checked, then somone has written outside
  of the memory block they have been allocated. It is up
  to you to call the `umm_poison_check()` function.

## API

The following functions are available for your application:

```c
void *umm_malloc(size_t size)
void *umm_calloc(size_t num, size_t size)
void *umm_realloc(void *ptr, size_t size)
void  umm_free(void *ptr)
```

They have exactly the same semantics as the corresponding standard library
functions.

To initialize the library there are two options:

```c
void  umm_init(void)
void  umm_init_heap(void *ptr, size_t size)
```

### Multi-Heap API

For the case of multiple heaps, corresponding `umm_multi_*` functions are provided.

```c
void *umm_multi_malloc(umm_heap *heap, size_t size)
void *umm_multi_calloc(umm_heap *heap, size_t num, size_t size)
void *umm_multi_realloc(umm_heap *heap, void *ptr, size_t size)
void  umm_multi_free(umm_heap *heap, void *ptr)
```

As with the standard API, there are two options for initialization:

```c
void  umm_multi_init(umm_heap *heap)
void  umm_multi_init_heap(umm_heap *heap, void *ptr, size_t size)
```
 
## Background

The memory manager assumes the following things:

1. The standard POSIX compliant malloc/calloc/realloc/free semantics are used
1. All memory used by the manager is allocated at link time, it is aligned
on a 32 bit boundary, it is contiguous, and its extent (start and end
address) is filled in by the linker.
1. All memory used by the manager is initialized to 0 as part of the
runtime startup routine. No other initialization is required.

The fastest linked list implementations use doubly linked lists so that
its possible to insert and delete blocks in constant time. This memory
manager keeps track of both free and used blocks in a doubly linked list.

Most memory managers use a list structure made up of pointers
to keep track of used - and sometimes free - blocks of memory. In an
embedded system, this can get pretty expensive as each pointer can use
up to 32 bits.

In most embedded systems there is no need for managing a large quantity
of memory block dynamically, so a full 32 bit pointer based data structure
for the free and used block lists is wasteful. A block of memory on
the free list would use 16 bytes just for the pointers!

This memory management library sees the heap as an array of blocks,
and uses block numbers to keep track of locations. The block numbers are
15 bits - which allows for up to 32767 blocks of memory. The high order
bit marks a block as being either free or in use, which will be explained
later.

The result is that a block of memory on the free list uses just 8 bytes
instead of 16.

In fact, we go even one step futher when we realize that the free block
index values are available to store data when the block is allocated.

The overhead of an allocated block is therefore just 4 bytes.

Each memory block holds 8 bytes, and there are up to 32767 blocks
available, for about 256K of heap space. If that's not enough, you
can always add more data bytes to the body of the memory block
at the expense of free block size overhead.

There are a lot of little features and optimizations in this memory
management system that makes it especially suited to small systems, and
the best way to appreciate them is to review the data structures and
algorithms used, so let's get started.

## Detailed Description

We have a general notation for a block that we'll use to describe the
different scenarios that our memory allocation algorithm must deal with:

```
   +----+----+----+----+
c  |* n |  p | nf | pf |
   +----+----+----+----+
```

Where:

- c  is the index of this block
- *  is the indicator for a free block
- n  is the index of the next block in the heap
- p  is the index of the previous block in the heap
- nf is the index of the next block in the free list
- pf is the index of the previous block in the free list

The fact that we have forward and backward links in the block descriptors
means that malloc() and free() operations can be very fast. It's easy
to either allocate the whole free item to a new block or to allocate part
of the free item and leave the rest on the free list without traversing
the list from front to back first.

The entire block of memory used by the heap is assumed to be initialized
to 0. The very first block in the heap is special - it't the head of the
free block list. It is never assimilated with a free block (more on this
later).

Once a block has been allocated to the application, it looks like this:

```
  +----+----+----+----+
c |  n |  p |   ...   |
  +----+----+----+----+
```

Where:

- c  is the index of this block
- n  is the index of the next block in the heap
- p  is the index of the previous block in the heap

Note that the free list information is gone because it's now
being used to store actual data for the application. If we had
even 500 items in use, that would be 2,000 bytes for
free list information. We simply can't afford to waste that much.

The address of the `...` area is what is returned to the application
for data storage.

The following sections describe the scenarios encountered during the
operation of the library. There are two additional notation conventions:

`??` inside a pointer block means that the data is irrelevant. We don't care
about it because we don't read or modify it in the scenario being
described.

`...` between memory blocks indicates zero or more additional blocks are
allocated for use by the upper block.

While we're talking about "upper" and "lower" blocks, we should make
a comment about adresses. In the diagrams, a block higher up in the
picture is at a lower address. And the blocks grow downwards their
block index increases as does their physical address.

Finally, there's one very important characteristic of the individual
blocks that make up the heap - there can never be two consecutive free
memory blocks, but there can be consecutive used memory blocks.

The reason is that we always want to have a short free list of the
largest possible block sizes. By always assimilating a newly freed block
with adjacent free blocks, we maximize the size of each free memory area.

### Operation of malloc right after system startup

As part of the system startup code, all of the heap has been cleared.

During the very first malloc operation, we start traversing the free list
starting at index 0. The index of the next free block is 0, which means
we're at the end of the list!

At this point, the malloc has a special test that checks if the current
block index is 0, which it is. This special case initializes the free
list to point at block index 1 and then points block 1 to the
last block (lf) on the heap.

```
   BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+
0  |  0 |  0 |  0 |  0 |           0  |  1 |  0 |  1 |  1 |
   +----+----+----+----+              +----+----+----+----+
                                      +----+----+----+----+
                                   1  |*lf |  0 |  0 |  0 |
                                      +----+----+----+----+
                                               ...
                                      +----+----+----+----+
                                   lf |  0 |  1 |  0 |  0 |
                                      +----+----+----+----+
```

The heap is now ready to complete the first malloc operation.

### Operation of malloc when we have reached the end of the free list and there is no block large enough to accommodate the request.

This happens at the very first malloc operation, or any time the free
list is traversed and no free block large enough for the request is
found.

The current block pointer will be at the end of the free list, and we
know we're at the end of the list because the nf index is 0, like this:

```
   BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+
pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | lf | ?? |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
 p | cf | ?? |   ...   |            p | cf | ?? |   ...   |
   +----+----+----+----+              +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+  
cf |  0 |  p |  0 | pf |            c | lf |  p |   ...   | 
   +----+----+----+----+              +----+----+----+----+
                                      +----+----+----+----+
                                   lf |  0 | cf |  0 | pf |
                                      +----+----+----+----+
```

As we walk the free list looking for a block of size b or larger, we get
to cf, which is the last item in the free list. We know this because the
next index is 0.

So we're going to turn cf into the new block of memory, and then create
a new block that represents the last free entry (lf) and adjust the prev
index of lf to point at the  block we just created. We also need to adjust
the next index of the new block (c) to point to the last free block.

Note that the next free index of the pf block must point to the new lf
because cf is no longer a free block!

### Operation of malloc when we have found a block (cf) that will fit the current request of b units exactly

This one is pretty easy, just clear the free list bit in the current
block and unhook it from the free list.

```
   BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+
pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | nf | ?? |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
 p | cf | ?? |   ...   |            p | cf | ?? |   ...   |
   +----+----+----+----+              +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+  Clear the free
cf |* n |  p | nf | pf |           cf |  n |  p |   ..    |  list bit here
   +----+----+----+----+              +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+
 n | ?? | cf |   ...   |            n | ?? | cf |   ...   |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
nf |*?? | ?? | ?? | cf |           nf | ?? | ?? | ?? | pf |
   +----+----+----+----+              +----+----+----+----+
```

Unhooking from the free list is accomplished by adjusting the next and
prev free list index values in the pf and nf blocks.

### Operation of malloc when we have found a block that will fit the current request of b units with some left over

We'll allocate the new block at the END of the current free block so we
don't have to change ANY free list pointers.

```
   BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+
pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | cf | ?? |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
 p | cf | ?? |   ...   |            p | cf | ?? |   ...   |
   +----+----+----+----+              +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+
cf |* n |  p | nf | pf |           cf |* c |  p | nf | pf |
   +----+----+----+----+              +----+----+----+----+
                                      +----+----+----+----+ This is the new
                                    c |  n | cf |   ..    | block at cf+b
                                      +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+
 n | ?? | cf |   ...   |            n | ?? |  c |   ...   |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
nf |*?? | ?? | ?? | cf |           nf | ?? | ?? | ?? | pf |
   +----+----+----+----+              +----+----+----+----+
```

This one is prety easy too, except we don't need to mess with the
free list indexes at all becasue we'll allocate the new block at the
end of the current free block. We do, however have to adjust the
indexes in cf, c, and n.

That covers the initialization and all possible malloc scenarios, so now
we need to cover the free operation possibilities...

### Free Scenarios

The operation of free depends on the position of the current block being
freed relative to free list items immediately above or below it. The code
works like this:

```
if next block is free
    assimilate with next block already on free list
if prev block is free
    assimilate with prev block already on free list
else
    put current block at head of free list
```

Step 1 of the free operation checks if the next block is free, and if it
is assimilate the next block with this one.

Note that c is the block we are freeing up, cf is the free block that
follows it.

```
   BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+
pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | nf | ?? |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
 p |  c | ?? |   ...   |            p |  c | ?? |   ...   |
   +----+----+----+----+              +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+ This block is
 c | cf |  p |   ...   |            c | nn |  p |   ...   | disconnected
   +----+----+----+----+              +----+----+----+----+ from free list,
   +----+----+----+----+                                    assimilated with
cf |*nn |  c | nf | pf |                                    the next, and
   +----+----+----+----+                                    ready for step 2
   +----+----+----+----+              +----+----+----+----+
nn | ?? | cf | ?? | ?? |           nn | ?? |  c |   ...   |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
nf |*?? | ?? | ?? | cf |           nf |*?? | ?? | ?? | pf |
   +----+----+----+----+              +----+----+----+----+
```

Take special note that the newly assimilated block (c) is completely
disconnected from the free list, and it does not have its free list
bit set. This is important as we move on to step 2 of the procedure...

Step 2 of the free operation checks if the prev block is free, and if it
is then assimilate it with this block.

Note that c is the block we are freeing up, pf is the free block that
precedes it.

```
   BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+ This block has
pf |* c | ?? | nf | ?? |           pf |* n | ?? | nf | ?? | assimilated the
   +----+----+----+----+              +----+----+----+----+ current block
   +----+----+----+----+
 c |  n | pf |   ...   |
   +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+
 n | ?? |  c |   ...   |            n | ?? | pf | ?? | ?? |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
nf |*?? | ?? | ?? | pf |           nf |*?? | ?? | ?? | pf |
   +----+----+----+----+              +----+----+----+----+
```

Nothing magic here, except that when we're done, the current block (c)
is gone since it's been absorbed into the previous free block. Note that
the previous step guarantees that the next block (n) is not free.

Step 3 of the free operation only runs if the previous block is not free.
it just inserts the current block to the head of the free list.

Remember, 0 is always the first block in the memory heap, and it's always
head of the free list!

```
   BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+
 0 | ?? | ?? | nf |  0 |            0 | ?? | ?? |  c |  0 |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
 p |  c | ?? |   ...   |            p |  c | ?? |   ...   |
   +----+----+----+----+              +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+
 c |  n |  p |   ..    |            c |* n |  p | nf |  0 |
   +----+----+----+----+              +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+
 n | ?? |  c |   ...   |            n | ?? |  c |   ...   |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
nf |*?? | ?? | ?? |  0 |           nf |*?? | ?? | ?? |  c |
   +----+----+----+----+              +----+----+----+----+
```

Again, nothing spectacular here, we're simply adjusting a few pointers
to make the most recently freed block the first item in the free list.

That's because finding the previous free block would mean a reverse
traversal of blocks until we found a free one, and it's just easier to
put it at the head of the list. No traversal is needed.

### Realloc Scenarios

Finally, we can cover realloc, which has the following basic operation.

The first thing we do is assimilate up with the next free block of
memory if possible. This step might help if we're resizing to a bigger
block of memory. It also helps if we're downsizing and creating a new
free block with the leftover memory.

First we check to see if the next block is free, and we assimilate it
to this block if it is. If the previous block is also free, and if
combining it with the current block would satisfy the request, then we
assimilate with that block and move the current data down to the new
location.

Assimilating with the previous free block and moving the data works
like this:

```
   BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+
pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | nf | ?? |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
cf |* c | ?? | nf | pf |            c |  n | ?? |   ...   | The data gets
   +----+----+----+----+              +----+----+----+----+ moved from c to
   +----+----+----+----+                                    the new data area  
 c |  n | cf |   ...   |                                    in cf, then c is
   +----+----+----+----+                                    adjusted to cf
   +----+----+----+----+              +----+----+----+----+
 n | ?? |  c |   ...   |            n | ?? |  c | ?? | ?? |
   +----+----+----+----+              +----+----+----+----+
            ...                                ...
   +----+----+----+----+              +----+----+----+----+
nf |*?? | ?? | ?? | cf |           nf |*?? | ?? | ?? | pf |
   +----+----+----+----+              +----+----+----+----+
```

Once we're done that, there are three scenarios to consider:

1. The current block size is exactly the right size, so no more work is
needed.

2. The current block is bigger than the new required size, so carve off
the excess and add it to the free list.

3. The current block is still smaller than the required size, so malloc
a new block of the correct size and copy the current data into the new
block before freeing the current block.

The only one of these scenarios that involves an operation that has not
yet been described is the second one, and it's shown below:

```
BEFORE                             AFTER

   +----+----+----+----+              +----+----+----+----+
 p |  c | ?? |   ...   |            p |  c | ?? |   ...   |
   +----+----+----+----+              +----+----+----+----+
   +----+----+----+----+              +----+----+----+----+
 c |  n |  p |   ...   |            c |  s |  p |   ...   |
   +----+----+----+----+              +----+----+----+----+
                                      +----+----+----+----+ This is the
                                    s |  n |  c |   ..    | new block at
                                      +----+----+----+----+ c+blocks
   +----+----+----+----+              +----+----+----+----+
 n | ?? |  c |   ...   |            n | ?? |  s |   ...   |
   +----+----+----+----+              +----+----+----+----+
```

Then we call free() with the adress of the data portion of the new
block (s) which adds it to the free list.