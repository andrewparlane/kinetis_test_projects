#include "ft81x.h"
#include "ft81x/g_ram_manager.h"

#include <stdlib.h>

ft81x_result ft81x_g_ram_manager_initialise(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    // initialise the head of the linked list
    // address is 0
    // and we are not allocated.
    handle->g_ram_manager_data.head.address_and_flags = (0 & (FT81X_G_RAM_MANAGER_ADDRESS_MASK)) |
                                               ((FT81X_G_RAM_MANAGER_FREE) & (FT81X_G_RAM_MANAGER_ALLOCATED_MASK));

    // there is no next node
    handle->g_ram_manager_data.head.next = NULL;

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_g_ram_manager_allocate(FT81X_Handle *handle, uint32_t count, uint32_t *offset)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    // look through the linked list trying to find a big enough unalloctade region
    GRAM_Linked_List_Node *node = &(handle->g_ram_manager_data.head);
    while (node)
    {
        // is this node free?
        if ((node->address_and_flags & (FT81X_G_RAM_MANAGER_ALLOCATED_MASK)) == (FT81X_G_RAM_MANAGER_ALLOCATED))
        {
            // nope
            node = node->next;
            continue;
        }

        // calculate size of node
        uint32_t nextAddress;
        if (node->next)
        {
            nextAddress = node->next->address_and_flags & (FT81X_G_RAM_MANAGER_ADDRESS_MASK);
        }
        else
        {
            nextAddress = FT81X_G_RAM_SIZE;
        }
        uint32_t available = nextAddress - (node->address_and_flags & (FT81X_G_RAM_MANAGER_ADDRESS_MASK));

        // is there enough space?
        if (available < count)
        {
            // nope
            node = node->next;
            continue;
        }

        // we can fit into this node
        // so mark this node as allocated
        node->address_and_flags = (node->address_and_flags & ~(FT81X_G_RAM_MANAGER_ALLOCATED_MASK)) |
                                  ((FT81X_G_RAM_MANAGER_ALLOCATED) & (FT81X_G_RAM_MANAGER_ALLOCATED_MASK));

        // insert a new node if needed
        if (available > (count + (FT81X_G_RAM_MANAGER_MINIMUM_ALLOC_SIZE)))
        {
            // work out the address for the next node
            uint32_t next_free_addr = (node->address_and_flags & (FT81X_G_RAM_MANAGER_ADDRESS_MASK)) + count;

            // alloctaed a new node
            GRAM_Linked_List_Node *new_node = (GRAM_Linked_List_Node *)malloc(sizeof(GRAM_Linked_List_Node));
            if (new_node == NULL)
            {
                return FT81X_RESULT_OUT_OF_MEMORY;
            }

            // update the new node
            // next_free_addr + 1 & (FT81X_G_RAM_MANAGER_ADDRESS_MASK)
            //  ensures alignment to 16 bits
            new_node->address_and_flags = ((next_free_addr + 1) & (FT81X_G_RAM_MANAGER_ADDRESS_MASK)) |
                                          ((FT81X_G_RAM_MANAGER_FREE) & (FT81X_G_RAM_MANAGER_ALLOCATED_MASK));
            new_node->next = node->next;

            // update current node to point at this new node
            node->next = new_node;
        }

        *offset = node->address_and_flags & (FT81X_G_RAM_MANAGER_ADDRESS_MASK);
        return FT81X_RESULT_OK;
    }

    // looked through everything, no space left
#warning TODO: defrag g_ram?
    // we have to somehow tell the user to update the address in the DL.

    return FT81X_RESULT_OUT_OF_G_RAM;
}
