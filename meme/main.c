#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#define HEAP_CAP 640000
#define BLOCK_LIST_CAP 1024

typedef struct
{
	char* start;
	size_t size;
	bool isactive;
}Block;

typedef struct
{
	size_t count;
	Block blocks[BLOCK_LIST_CAP];
}Block_List;

void block_list_show(const Block_List *list)
{
	printf("Allocated Blocks (%zu):\n", list->count);
	for (size_t i = 0; i < list->count; ++i)
	{
		printf("  adress: %p, size: %zu, status: ",
			list->blocks[i].start,
			list->blocks[i].size);
		if (list->blocks[i].isactive == true)
			printf("active\n");
		else
			printf("inactive\n");
	}
}

void block_list_show_free(const Block_List* list)
{
	printf("Freed Blocks (%zu):\n", list->count);
	for (size_t i = 0; i < list->count; ++i)
	{
		printf("  adress: %p, size: %zu, status: ",
			list->blocks[i].start,
			list->blocks[i].size);
		if (list->blocks[i].isactive == false)
			printf("active\n");
		else
			printf("inactive\n");
	}
}

void block_list_insert(Block_List* list, void* start, size_t size)
{
	assert(list->count < BLOCK_LIST_CAP);
	list->blocks[list->count].start = start;
	list->blocks[list->count].size = size;
	list->blocks[list->count].isactive = true;

	for (size_t i = list->count; i > 0 && 
		list->blocks[i].start < list->blocks[i-1].start; --i)
	{
		const Block temp = list->blocks[i];
		list->blocks[i] = list->blocks[i - 1];
		list->blocks[i - 1] = temp;
	}

	list->count += 1;
}


int block_list_find(const Block_List *list, void* ptr)
{
	for (size_t i = 0; i < list->count; ++i) {
		if (list->blocks[i].start == ptr) {
			return (int)i;
		}
	}
	return -1;
}

void block_list_remove(Block_List *list, size_t index)
{
	assert(index < list->count);
	for (size_t i = index; i < list->count; ++i)
	{
		list->blocks[i] = list->blocks[i + 1];
	}
	list->count -= 1;
}



char heap[HEAP_CAP] = { 0 };

Block_List alloced_blocks = {0};
Block_List freed_blocks = 
{ 
	.count = 1,
	.blocks = { [0] = {.start = heap, .size = sizeof(heap)}}
};

void* heap_alloc(size_t size)
{
	if (size > 0)
	{
		for (size_t i = 0; i < freed_blocks.count; ++i)
		{
			const Block block = freed_blocks.blocks[i];
			if (block.size > size)
			{
				block_list_remove(&freed_blocks, i);

				void* const ptr = block.start;
				const size_t tail_size = block.size - size;
				block_list_insert(&alloced_blocks, block.start, size);

				if (tail_size > 0)
				{
					block_list_insert(&freed_blocks, block.start + size, tail_size);
				}

				return block.start;
			}
		}
	} 

	return NULL;
}



void heap_free(void* ptr)
{
	if (ptr != NULL)
	{
		const int index = block_list_find(&alloced_blocks, ptr);
		assert(index >= 0);
		block_list_insert(&freed_blocks,
			alloced_blocks.blocks[index].start,
			alloced_blocks.blocks[index].size);
		block_list_remove(&alloced_blocks, (size_t)index);
	}
}

void heap_collect()
{
	
}

int main()
{

	for (int i = 0; i < 5; ++i)
	{
		void* p = heap_alloc(i);
	}

	heap_alloc(420);

	
	block_list_show(&alloced_blocks);
	block_list_show_free(&freed_blocks);

	return 0;
}