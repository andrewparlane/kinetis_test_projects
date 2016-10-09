#include "ft81x.h"
#include "ft81x/g_ram_manager.h"
#include "ft81x/platforms/platform.h"

#include <stdio.h>
#include <stdlib.h>

// some defines for use by the G_RAM manager
#define ADDRESS_MASK        0x0FFFFE
#define ALLOCATED_MASK      0x000001
#define NODE_ALLOCATED      1
#define NODE_FREE           0

// minimum allocation size (no point having a node with only 1 byte available)
#define MINIMUM_ALLOC_SIZE  32


// ----------------------------------------------------------------------------
// API functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_g_ram_manager_initialise(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    // initialise the head of the linked list
    // address is 0
    // and we are not allocated.
    handle->g_ram_manager_data.head.address_and_flags = (0 & (ADDRESS_MASK)) |
                                                        ((NODE_FREE) & (ALLOCATED_MASK));

    // there is no next node nor previous one
    handle->g_ram_manager_data.head.next = NULL;
    handle->g_ram_manager_data.head.prev = NULL;

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_g_ram_manager_allocate(FT81X_Handle *handle, uint32_t count, uint32_t *offset)
{
    // look through the linked list trying to find a big enough unalloctade region
    GRAM_Linked_List_Node *node = &(handle->g_ram_manager_data.head);
    while (node)
    {
        // is this node free?
        if ((node->address_and_flags & (ALLOCATED_MASK)) == (NODE_ALLOCATED))
        {
            // nope
            node = node->next;
            continue;
        }

        // calculate size of node
        uint32_t nextAddress;
        if (node->next)
        {
            nextAddress = node->next->address_and_flags & (ADDRESS_MASK);
        }
        else
        {
            nextAddress = FT81X_G_RAM_SIZE;
        }
        uint32_t available = nextAddress - (node->address_and_flags & (ADDRESS_MASK));

        // is there enough space?
        if (available < count)
        {
            // nope
            node = node->next;
            continue;
        }

        // we can fit into this node
        // so mark this node as allocated
        node->address_and_flags = (node->address_and_flags & ~(ALLOCATED_MASK)) |
                                  ((NODE_ALLOCATED) & (ALLOCATED_MASK));

        // insert a new node if needed
        if (available > (count + (MINIMUM_ALLOC_SIZE)))
        {
            // work out the address for the next node
            uint32_t next_free_addr = (node->address_and_flags & (ADDRESS_MASK)) + count;

            // alloctaed a new node
            GRAM_Linked_List_Node *new_node = (GRAM_Linked_List_Node *)malloc(sizeof(GRAM_Linked_List_Node));
            if (new_node == NULL)
            {
                return FT81X_RESULT_OUT_OF_MEMORY;
            }

            // update the new node
            // next_free_addr + 1 & (ADDRESS_MASK)
            //  ensures alignment to 16 bits
            new_node->address_and_flags = ((next_free_addr + 1) & (ADDRESS_MASK)) |
                                          ((NODE_FREE) & (ALLOCATED_MASK));
            new_node->next = node->next;
            new_node->prev = node;

            // update next node's prev pointer to point at this node
            if (new_node->next)
            {
                new_node->next->prev = new_node;
            }

            // update current node to point at this new node
            node->next = new_node;
        }

        *offset = node->address_and_flags & (ADDRESS_MASK);
        return FT81X_RESULT_OK;
    }

    // looked through everything, no space left
#warning TODO: defrag g_ram?
    // we have to somehow tell the user to update the address in the DL.

    return FT81X_RESULT_OUT_OF_G_RAM;
}

ft81x_result ft81x_g_ram_manager_write(FT81X_Handle *handle, uint32_t offset, uint32_t count, const uint8_t *data)
{
    WRITE_GPU_MEM(((FT81X_G_RAM) + offset), count, data);

    return FT81X_RESULT_OK;
}

void ft81x_g_ram_manager_dump_nodes(FT81X_Handle *handle)
{
    GRAM_Linked_List_Node *node = &(handle->g_ram_manager_data.head);
    while (node)
    {
        printf("node: %08X\n", (unsigned int)node);
        printf("      offset: %08X\n", (unsigned int)(node->address_and_flags & ADDRESS_MASK));
        printf("      %s\n", (node->address_and_flags & ALLOCATED_MASK) ? "allocated" : "free");
        printf("      next: %08X\n", (unsigned int)node->next);
        printf("      prev: %08X\n", (unsigned int)node->prev);
        printf("\n");

        node = node->next;
    }
}
