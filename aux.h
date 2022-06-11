#ifndef AUX_H
#define AUX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "request.h"
#include <sys/time.h>



typedef struct node
{
    int connection_fd;
    struct node *next;
    struct node *prev;
    struct timeval stat_req_arrival;
    struct timeval stat_req_dispatch;
} * Node;

typedef struct thread_info
{
    Node request_node;
    pid_t thread_id;
    int thread_index;
    int thread_count;
    int thread_static_count;
    int thread_dynamic_count;

    char* sched_policy;
}*ThreadInfo;

typedef struct queue
{
    Node head;
    Node tail;
    unsigned int size;
} * Queue;

Queue createQueue();
bool pushQueue(Queue q, Node n);
Node popQueue(Queue q);
void displayQueue(Queue q);
void deleteQueue(Queue q);
void removeRandom(Queue q);

typedef struct linked_list
{
    Node head;
    Node tail;
    unsigned int size;
} * LinkedList;

LinkedList createLinkedList();
bool insertLinkedList(LinkedList ll, Node n);
void displayLinkedList(LinkedList ll);
bool removeFromLinkedList(LinkedList ll, Node n);
void deleteLinkedList(LinkedList ll);

#endif