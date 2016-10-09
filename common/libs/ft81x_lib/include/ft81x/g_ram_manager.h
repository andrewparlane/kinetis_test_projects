#ifndef __G_RAM_MANAGER_H
#define __G_RAM_MANAGER_H

#include "ft81x/memory_map.h"
#include <stdint.h>


typedef struct _GRAM_Linked_List_Node
{
    uint32_t address_and_flags;
    struct _GRAM_Linked_List_Node *next;
    struct _GRAM_Linked_List_Node *prev;
} GRAM_Linked_List_Node;

typedef struct
{
    GRAM_Linked_List_Node head;
} FT81X_G_RAM_Manager_Data;

// forward declarations for ft81x.h
enum _ft81x_result;
typedef enum _ft81x_result ft81x_result;
struct _FT81X_Handle;
typedef struct _FT81X_Handle FT81X_Handle;

ft81x_result ft81x_g_ram_manager_initialise(FT81X_Handle *handle);
ft81x_result ft81x_g_ram_manager_allocate(FT81X_Handle *handle, uint32_t count, uint32_t *offset);
ft81x_result ft81x_g_ram_manager_write(FT81X_Handle *, uint32_t offset, uint32_t count, const uint8_t *data);

void ft81x_g_ram_manager_dump_nodes(FT81X_Handle *handle);

#endif
