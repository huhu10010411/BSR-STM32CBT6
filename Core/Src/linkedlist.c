/*
 * linklist.c
 *
 *  Created on: Oct 27, 2023
 *      Author: Admin
 */

#include "linkedlist.h"

SensorNode_t SSnode_default = SENSORNODE_T_INIT;

Node* create_node(SensorNode_t  SSnode)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->SSnode= SSnode;
    node->next = NULL;
    return node;
}

s_list * list_create(void)
{
    s_list *list = (s_list*)malloc(sizeof(s_list));
//    assert(list != NULL);
    list->head = create_node(SSnode_default);
    list->tail = create_node(SSnode_default);
    list->head->next = list->tail;
    list->length = 0;
    return list;
}

void list_append (s_list* list, SensorNode_t SSnode_data)
{
	list->tail->SSnode = SSnode_data;
	Node *new_tail = create_node(SSnode_default);
	list->tail->next = new_tail;
	list->tail = new_tail;
	list->length += 1;
}

