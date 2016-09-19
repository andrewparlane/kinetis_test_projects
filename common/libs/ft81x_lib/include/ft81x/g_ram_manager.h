#ifndef __G_RAM_MANAGER_H
#define __G_RAM_MANAGER_H

#include "ft81x/memory_map.h"
#include <stdint.h>

// some defines for use by the G_RAM manager
#define FT81X_G_RAM_MANAGER_ADDRESS_MASK        0x0FFFFE
#define FT81X_G_RAM_MANAGER_ALLOCATED_MASK      0x000001
#define FT81X_G_RAM_MANAGER_ALLOCATED           1
#define FT81X_G_RAM_MANAGER_FREE                0

// minimum allocation size (no point having a node with only 1 byte available)
#define FT81X_G_RAM_MANAGER_MINIMUM_ALLOC_SIZE  32

typedef struct _GRAM_Linked_List_Node
{
    uint32_t address_and_flags;
    struct _GRAM_Linked_List_Node *next;
} GRAM_Linked_List_Node;

typedef struct
{
    GRAM_Linked_List_Node head;
} GRAM_Info;

// forward declarations for ft81x.h
enum _ft81x_result;
typedef enum _ft81x_result ft81x_result;
struct _FT81X_Handle;
typedef struct _FT81X_Handle FT81X_Handle;

ft81x_result ft81x_g_ram_manager_initialise(FT81X_Handle *handle);
ft81x_result ft81x_g_ram_manager_allocate(FT81X_Handle *handle, uint32_t count, uint32_t *offset);

#endif
