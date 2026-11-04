#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stddef.h>

#define MAX_POOL_SIZE 1000000

typedef struct {
    size_t start;
    size_t size;
    int allocation_id;
    int is_free;
} Block;

typedef struct {
    unsigned char *pool;
    Block *blocks;
    size_t total_size;
    int count;
    int capacity;
    int next_id;
    int strategy;
} MemoryManager;


// Release all dynamically allocated memory
void cleanup(MemoryManager *manager) {

    free(manager->pool);
    free(manager->blocks);

    manager->pool = NULL;
    manager->blocks = NULL;
}

// Display menu
void display_menu(void) {

    printf("\n========== SMART MEMORY MANAGER ==========\n");

    printf("1. Allocate memory\n");
    printf("2. Release memory\n");
    printf("3. Resize allocation\n");
    printf("4. Display memory map\n");
    printf("5. Display memory statistics\n");
    printf("6. Change allocation strategy\n");
    printf("7. Exit\n");

    printf("Choice: ");
}

// Change allocation strategy
void change_strategy(MemoryManager *manager) {

    int user_choice;

    printf("\nSelect allocation strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    printf("Choice: ");

    if (scanf("%d", &user_choice) != 1) {
        printf("Invalid input.\n");
        return;
    }

    if (user_choice < 1 || user_choice > 3) {
        printf("Invalid strategy!\n");
        return;
    }

    manager->strategy = user_choice;

    if (user_choice == 1) {
        printf("Strategy set to First Fit.\n");
    }
    else if (user_choice == 2) {
        printf("Strategy set to Best Fit.\n");
    }
    else {
        printf("Strategy set to Worst Fit.\n");
    }
}

// Display memory map
void display_memory_map(MemoryManager *manager) {

    printf("\nMemory Map (%zu bytes):\n", manager->total_size);

    for (int index = 0; index < manager->count; index++) {

        Block current_block = manager->blocks[index];

        if (current_block.is_free) {

            printf(
                "[Free: %zu bytes, Start: %zu]\n",
                current_block.size,
                current_block.start
            );
        }
        else {

            printf(
                "[Allocated #%d: %zu bytes, Start: %zu]\n",
                current_block.allocation_id,
                current_block.size,
                current_block.start
            );
        }
    }
}

// Display memory statistics
void display_statistics(MemoryManager *manager) {

    size_t total_allocated = 0;
    size_t total_free = 0;
    size_t largest_free_chunk = 0;

    int free_chunk_count = 0;

    for (int index = 0; index < manager->count; index++) {

        if (manager->blocks[index].is_free) {

            total_free += manager->blocks[index].size;
            free_chunk_count++;

            if (manager->blocks[index].size > largest_free_chunk) {
                largest_free_chunk = manager->blocks[index].size;
            }
        }
        else {

            total_allocated += manager->blocks[index].size;
        }
    }

    printf("\nMemory Statistics\n");

    printf("Total memory: %zu bytes\n", manager->total_size);
    printf("Allocated: %zu bytes\n", total_allocated);
    printf("Free: %zu bytes\n", total_free);

    printf("Free blocks: %d\n", free_chunk_count);

    printf("Largest free block: %zu bytes\n", largest_free_chunk);
}

// Detect external fragmentation
void detect_fragmentation(MemoryManager *manager,
                          size_t requested_size) {

    size_t total_free_space = 0;
    size_t biggest_free_chunk = 0;

    for (int index = 0; index < manager->count; index++) {

        if (manager->blocks[index].is_free) {

            total_free_space += manager->blocks[index].size;

            if (manager->blocks[index].size > biggest_free_chunk) {
                biggest_free_chunk = manager->blocks[index].size;
            }
        }
    }
}

// Grow metadata array using realloc
int grow_metadata(MemoryManager *manager) {

    if (manager->count < manager->capacity) {
        return 1;
    }

    int new_capacity = manager->capacity * 2;

    Block *expanded_blocks = realloc(
        manager->blocks,
        new_capacity * sizeof(Block)
    );

    if (expanded_blocks == NULL) {
        return 0;
    }

    manager->blocks = expanded_blocks;
    manager->capacity = new_capacity;

    return 1;
}

// Insert a block into metadata array
int insert_block(MemoryManager *manager, int position, Block new_block) {

    if (!grow_metadata(manager)) {
        return 0;
    }

    for (int index = manager->count; index > position; index--) {
        manager->blocks[index] = manager->blocks[index - 1];
    }

    manager->blocks[position] = new_block;
    manager->count++;

    return 1;
}

// Remove a block from metadata array
void remove_block(MemoryManager *manager, int position) {

    for (int index = position; index < manager->count - 1; index++) {
        manager->blocks[index] = manager->blocks[index + 1];
    }

    manager->count--;
}

// Merge adjacent free blocks
void merge_free_blocks(MemoryManager *manager) {

    int index = 0;

    while (index < manager->count - 1) {

        if (manager->blocks[index].is_free &&
            manager->blocks[index + 1].is_free) {

            manager->blocks[index].size += manager->blocks[index + 1].size;

            remove_block(manager, index + 1);

        }
        else {
            index++;
        }
    }
}

// Find allocation using ID
int find_allocation(MemoryManager *manager, int target_id) {

    for (int index = 0; index < manager->count; index++) {

        if (!manager->blocks[index].is_free &&
            manager->blocks[index].allocation_id == target_id) {
            return index;
        }
    }

    return -1;
}

// Release memory
void release_memory(MemoryManager *manager, int target_id) {

    int block_index = find_allocation(manager, target_id);

    if (block_index == -1) {
        printf("Invalid allocation ID!\n");
        return;
    }

    manager->blocks[block_index].is_free = 1;
    manager->blocks[block_index].allocation_id = 0;

    merge_free_blocks(manager);

    printf("Allocation released.\n");
}

// Find suitable free block based on strategy
int find_free_block(MemoryManager *manager, size_t requested_size) {

    int chosen_index = -1;

    for (int index = 0; index < manager->count; index++) {

        if (!manager->blocks[index].is_free ||
            manager->blocks[index].size < requested_size) {
            continue;
        }

        if (manager->strategy == 1) {
            // First Fit
            return index;
        }

        if (chosen_index == -1) {
            chosen_index = index;
        }
        else if (manager->strategy == 2) {
            // Best Fit
            if (manager->blocks[index].size <
                manager->blocks[chosen_index].size) {
                chosen_index = index;
            }
        }
        else if (manager->strategy == 3) {
            // Worst Fit
            if (manager->blocks[index].size >
                manager->blocks[chosen_index].size) {
                chosen_index = index;
            }
        }
    }

    return chosen_index;
}

// Allocate inside a selected free block.
// Used by normal allocation and relocation during resize.
int allocate_at(MemoryManager *manager, int position,
                size_t requested_size, int unique_id) {

    Block *target_block = &manager->blocks[position];

    if (target_block->size == requested_size) {

        target_block->is_free = 0;
        target_block->allocation_id = unique_id;

        return 1;
    }

    Block new_allocation;

    new_allocation.start = target_block->start;
    new_allocation.size = requested_size;
    new_allocation.allocation_id = unique_id;
    new_allocation.is_free = 0;

    // Remaining free block
    Block leftover_space;

    leftover_space.start = target_block->start + requested_size;
    leftover_space.size = target_block->size - requested_size;
    leftover_space.allocation_id = 0;
    leftover_space.is_free = 1;

    // Insert remaining block first.
    // This keeps the original free block unchanged
    // if metadata allocation fails.
    if (!insert_block(manager, position + 1, leftover_space)) {
        return 0;
    }

    manager->blocks[position] = new_allocation;

    return 1;
}

// Allocate memory
void allocate_memory(MemoryManager *manager, size_t requested_size) {

    if (requested_size == 0 || requested_size > manager->total_size) {
        printf("Invalid allocation size!\n");
        return;
    }

    if (manager->next_id <= 0 || manager->next_id == INT_MAX) {
        printf("Allocation ID limit reached.\n");
        return;
    }

    int found_index = find_free_block(manager, requested_size);

    if (found_index == -1) {

        printf("Allocation failed: no contiguous free block is large enough.\n");

        detect_fragmentation(manager, requested_size);

        return;
    }

    int new_id = manager->next_id;

    if (!allocate_at(manager, found_index, requested_size, new_id)) {
        printf("Allocation failed: metadata memory unavailable.\n");
        return;
    }

    manager->next_id++;

    printf(
        "Allocation successful. ID: %d, Start: %zu, Size: %zu\n",
        new_id,
        manager->blocks[found_index].start,
        requested_size
    );
}


// Resize allocation
void resize_memory(MemoryManager *manager,
                   int target_id, size_t requested_new_size) {

    if (requested_new_size == 0 || requested_new_size > manager->total_size) {
        printf("Invalid resize size!\n");
        return;
    }

    int block_index = find_allocation(manager, target_id);

    if (block_index == -1) {
        printf("Invalid allocation ID!\n");
        return;
    }

    size_t original_size = manager->blocks[block_index].size;
    size_t original_start = manager->blocks[block_index].start;

    if (requested_new_size == original_size) {
        printf("Size is already the requested size.\n");
        return;
    }

    // CASE 1: Shrinking allocation
    if (requested_new_size < original_size) {

        Block freed_portion;

        freed_portion.start = original_start + requested_new_size;
        freed_portion.size = original_size - requested_new_size;
        freed_portion.allocation_id = 0;
        freed_portion.is_free = 1;

        // Ensure metadata capacity before modifying anything.
        if (!grow_metadata(manager)) {
            printf("Resize failed: metadata allocation error.\n");
            return;
        }

        manager->blocks[block_index].size = requested_new_size;

        if (!insert_block(manager, block_index + 1, freed_portion)) {
            // This should not fail because capacity was reserved.
            manager->blocks[block_index].size = original_size;
            printf("Resize failed.\n");
            return;
        }

        merge_free_blocks(manager);

        printf("Allocation #%d shrunk successfully.\n", target_id);

        return;
    }


    // CASE 2: Expand into adjacent free block
    if (block_index + 1 < manager->count &&
        manager->blocks[block_index + 1].is_free) {

        size_t extra_space_needed = requested_new_size - original_size;

        if (manager->blocks[block_index + 1].size >= extra_space_needed) {

            if (manager->blocks[block_index + 1].size == extra_space_needed) {

                manager->blocks[block_index].size = requested_new_size;

                remove_block(manager, block_index + 1);

            }
            else {

                manager->blocks[block_index].size = requested_new_size;

                manager->blocks[block_index + 1].start += extra_space_needed;
                manager->blocks[block_index + 1].size -= extra_space_needed;
            }

            printf("Allocation #%d expanded in place.\n", target_id);

            return;
        }
    }


    // CASE 3: Move allocation to another free region
    int new_location_index = find_free_block(manager, requested_new_size);

    if (new_location_index == -1) {

        printf("Resize failed: no suitable free region.\n");

        detect_fragmentation(manager, requested_new_size);

        return;
    }

    size_t new_starting_point = manager->blocks[new_location_index].start;

    // Reserve destination first. If metadata growth fails,
    // the original allocation remains unchanged.
    if (!grow_metadata(manager)) {
        printf("Resize failed: metadata allocation error.\n");
        return;
    }

    if (!allocate_at(manager, new_location_index, requested_new_size, target_id)) {
        printf("Resize failed.\n");
        return;
    }

    // Copy the existing data to the new location.
    memcpy(
        manager->pool + new_starting_point,
        manager->pool + original_start,
        original_size
    );

    // Find the old allocation by its original start.
    // There are temporarily two blocks with the same ID.
    for (int index = 0; index < manager->count; index++) {

        if (!manager->blocks[index].is_free &&
            manager->blocks[index].allocation_id == target_id &&
            manager->blocks[index].start == original_start) {

            manager->blocks[index].is_free = 1;
            manager->blocks[index].allocation_id = 0;

            break;
        }
    }

    merge_free_blocks(manager);

    printf("Allocation #%d moved and resized successfully.\n", target_id);
}


// Initialize memory manager
int initialize(MemoryManager *manager, size_t requested_size) {

    manager->pool = malloc(requested_size);

    if (manager->pool == NULL) {
        return 0;
    }

    memset(manager->pool, 0, requested_size);

    manager->capacity = 1;
    manager->count = 1;

    manager->blocks = malloc(manager->capacity * sizeof(Block));

    if (manager->blocks == NULL) {
        free(manager->pool);
        manager->pool = NULL;
        return 0;
    }

    manager->total_size = requested_size;
    manager->next_id = 1;

    // Initially, the entire pool is free.
    manager->blocks[0].start = 0;
    manager->blocks[0].size = requested_size;
    manager->blocks[0].allocation_id = 0;
    manager->blocks[0].is_free = 1;

    return 1;
}

// Main function
int main(void) {

    MemoryManager system_manager = {0};

    size_t pool_capacity;

    printf("Enter memory pool size: ");

    if (scanf("%zu", &pool_capacity) != 1 ||
        pool_capacity < 1 ||
        pool_capacity > MAX_POOL_SIZE) {

        printf("Invalid pool size!\n");
        return 0;
    }

    if (!initialize(&system_manager, pool_capacity)) {

        printf("Memory initialization failed!\n");
        cleanup(&system_manager);

        return 0;
    }

    int chosen_strategy;

    printf(
        "Select allocation strategy "
        "(1. First Fit, 2. Best Fit, 3. Worst Fit): "
    );

    if (scanf("%d", &chosen_strategy) != 1 ||
        chosen_strategy < 1 || chosen_strategy > 3) {

        printf("Invalid strategy!\n");
        cleanup(&system_manager);

        return 0;
    }

    system_manager.strategy = chosen_strategy;

    printf("Strategy set to %s \n", (chosen_strategy == 1) ? "First Fit" : (chosen_strategy == 2) ? "Best Fit" : "Worst Fit");

    int user_action;

    while (1) {

        display_menu();

        if (scanf("%d", &user_action) != 1) {
            printf("Invalid input!\n");
            break;
        }

        switch (user_action) {

            case 1: {

                size_t amount_to_allocate;

                printf("Requested size: ");

                if (scanf("%zu", &amount_to_allocate) != 1) {
                    printf("Invalid input!\n");
                    goto exit_program;
                }

                allocate_memory(&system_manager, amount_to_allocate);

                break;
            }

            case 2: {

                int target_id;

                printf("Allocation ID: ");

                if (scanf("%d", &target_id) != 1) {
                    printf("Invalid input!\n");
                    goto exit_program;
                }

                release_memory(&system_manager, target_id);

                break;
            }

            case 3: {

                int target_id;
                size_t new_amount;

                printf("Allocation ID: ");

                if (scanf("%d", &target_id) != 1) {
                    printf("Invalid input!\n");
                    goto exit_program;
                }

                printf("New size: ");

                if (scanf("%zu", &new_amount) != 1) {
                    printf("Invalid input!\n");
                    goto exit_program;
                }

                resize_memory(&system_manager, target_id, new_amount);

                break;
            }

            case 4:

                display_memory_map(&system_manager);

                break;

            case 5:

                display_statistics(&system_manager);

                break;

            case 6:

                change_strategy(&system_manager);

                break;

            case 7:

                printf("Exiting Memory Manager.\n");

                goto exit_program;

            default:

                printf("Invalid menu choice!\n");
        }
    }

exit_program:

    cleanup(&system_manager);

    return 0;
}