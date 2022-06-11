#include "aux.h"

#define AFTER_MALLOC(p)                    \
{                                           \
    if(p == NULL)                            \
    {                                         \
        printf("\nAllocation Error\n");          \
        return -1;                               \
    }                                            \
}                                                 \

/*====================================*/
/*================Queue===============*/
/*====================================*/

Queue createQueue()
{
    Queue q = (Queue)malloc(sizeof(struct queue));
    AFTER_MALLOC(q);
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

bool pushQueue(Queue q, Node n)
{
    n->next = NULL;
    n->prev = NULL;
    if(q->tail == NULL)
    {
        q->head = q->tail = n;
    }
    else
    {
        q->tail->prev = n;
        n->next = q->tail;
        q->tail = n;
    }
    q->size++;
    return true;
}

Node popQueue(Queue q)
{
    if(q->size == 0)
    {
        return NULL;
    }
    Node n = q->head;
    q->head = q->head->prev;
    if(q->head != NULL)
    {
        q->head->next = NULL;
    }
    q->size--;
    return n;
}

void displayQueue(Queue q)
{
    if(q->size == 0)
    {
        printf("\nThe Queue is empty\n");
        return;
    }
    Node n = q->head;
    printf("Head of the Queue\n");
    while (n != NULL)
    {
        printf("%d\n^\n|\n", n->connection_fd);
        n = n->prev;
    }
    printf("NULL\nTail of the Queue\n");
}

void deleteQueue(Queue q)
{
    if(q->size != 0)
    {
        Node current = q->head;
        Node temp = NULL;
        while(current != NULL)
        {
            current->next = NULL;
            temp = current->prev;
            free(current);
            current = temp;
        }
        q->head = q->tail = NULL;
        q->size = 0;
    }
    free(q);
}

void removeRandom(Queue q)
{
    int index = 0;
    srand(time(NULL));
    if (q->size > 1)
    {
        index = rand() % (q->size - 1);
    }
    Node curr=q->head;  
    for (int i = 0; i < index; i++)
    {
        curr= curr->next;
    }
    Node node_to_be_deleted = curr->next;
    Close(node_to_be_deleted->connection_fd);
    free(node_to_be_deleted);
    q->size--;
}

/*====================================*/
/*=============LinkedList=============*/
/*====================================*/


LinkedList createLinkedList()
{
    LinkedList ll = (LinkedList)malloc(sizeof(struct linked_list));
    AFTER_MALLOC(ll);
    ll->head = NULL;
    ll->tail = NULL;
    ll->size = 0;
    return ll;
}

bool insertLinkedList(LinkedList ll, Node n)
{
    n->next = NULL;
    n->prev = NULL;
    if(ll->tail == NULL)
    {
        ll->head = ll->tail = n;
    }
    else
    {
        ll->tail->prev = n;
        n->next = ll->tail;
        ll->tail = n;
    }
    ll->size++;
    return true;
}

bool removeFromLinkedList(LinkedList ll, Node n)
{
    if(n == NULL)
    {
        return false;
    }
    Node next = n->next;
    Node prev = n->prev;
    if(next != NULL)
    {
        next->prev = prev;
    }
    if(prev != NULL)
    {
        prev->next = next;
    }
    n->next = n->prev = NULL;
    n->connection_fd = 0;
    free(n);
    ll->size--;
    return true;
}

void displayLinkedList(LinkedList ll)
{
    if(ll->size == 0)
    {
        printf("\nThe Linked List is empty\n");
        return;
    }
    Node n = ll->head;
    printf("Head of the Linked List\n");
    while (n != NULL)
    {
        printf("%d\n^\n|\n", n->connection_fd);
        n = n->prev;
    }
    printf("NULL\nTail of the Linked List\n");
}

void deleteLinkedList(LinkedList ll)
{
    if(ll->size != 0)
    {
        Node current = ll->head;
        Node temp = NULL;
        while(current != NULL)
        {
            temp = current->prev;
            current->prev = current->next = NULL;
            current->connection_fd = 0;
            free(current);
            current = temp;
        }
        ll->head = ll->tail = NULL;
        ll->size = 0;
    }
    free(ll);
}
