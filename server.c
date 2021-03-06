#include "segel.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too

static Queue waiting_tasks;

pthread_mutex_t lock_queue;
pthread_cond_t requests_not_max, queue_not_empty;
void requestHandle(int fd, ThreadInfo curr_thread_info);

int tasks_count = 0;
int threads_number, req_number;


void getargs(int *port, int *threads_number, int *req_number, int argc, char *argv[])
{
    if(port == NULL || threads_number == NULL || req_number == NULL || argv == NULL)
    {
	    exit(1);
    }
    if (argc < 5) 
    {
	    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	    exit(1);
    }
    *port = atoi(argv[1]);
    *threads_number = atoi(argv[2]);
    if(*threads_number < 1)
    {
	    exit(1);
    }
    *req_number = atoi(argv[3]);
    if(*req_number < 1)
    {
	    exit(1);
    }
    if(argv[4] == NULL)
    {
        exit(1);
    }
}

void* thread_start_routine(void* thread_info)
{
    ThreadInfo curr_thread_info = (ThreadInfo)(thread_info);
    while(1)
    {
        pthread_mutex_lock(&lock_queue);
        while(waiting_tasks->size == 0)
        {
            pthread_cond_wait(&queue_not_empty, &lock_queue);
        }
        struct timeval temp;
        Node head_node = getHead(waiting_tasks);
        gettimeofday(&temp, NULL);
        head_node = popQueue(waiting_tasks);
        
        if(head_node == NULL)
        {
            pthread_mutex_unlock(&lock_queue);
            continue;
        }
        timersub(&temp,&head_node->stat_req_arrival, &head_node->stat_req_dispatch); 
        pthread_mutex_unlock(&lock_queue);
 
        curr_thread_info->request_node = head_node;

        requestHandle(curr_thread_info->request_node->connection_fd, curr_thread_info);

        Close(curr_thread_info->request_node->connection_fd);
    
        free(curr_thread_info->request_node);
        curr_thread_info->request_node = NULL;
        
        pthread_mutex_lock(&lock_queue);
        tasks_count--;
        pthread_cond_signal(&requests_not_max);
        pthread_mutex_unlock(&lock_queue);
        
    }
    free(curr_thread_info);
    return NULL;
} 


void initialize_task(Node request_node, char* sched_policy)
{
    pthread_mutex_lock(&lock_queue);
    if (tasks_count >= req_number)
    {
        if(strcmp(sched_policy ,"block") == 0)
        {            
            while(tasks_count >= req_number)
            {
                pthread_cond_wait(&requests_not_max, &lock_queue);
            }
        }
        else if(strcmp(sched_policy,"dt") == 0)
        {
            Close(request_node->connection_fd);
            free(request_node);
            pthread_mutex_unlock(&lock_queue);
            return;
        }
        else if (strcmp(sched_policy, "dh") == 0)
        {
            if(waiting_tasks->size == 0)
            {
                Close(request_node->connection_fd);
                free(request_node);
                pthread_mutex_unlock(&lock_queue);
                return;
            }
            Node request_to_be_deleted = popQueue(waiting_tasks);
            if(request_to_be_deleted != NULL)
            {
                Close(request_to_be_deleted->connection_fd);
                free(request_to_be_deleted);
                tasks_count--;
            }
            pthread_mutex_unlock(&lock_queue);
        }
        else if(!strcmp(sched_policy,"random"))
        {
            if(waiting_tasks->size == 0)
            {
                Close(request_node->connection_fd);
                free(request_node);
                pthread_mutex_unlock(&lock_queue);
                return;
            }
            int amount_to_remove = waiting_tasks->max_size * 3;
            if(amount_to_remove % 10 != 0)
            {
                amount_to_remove = (int)ceil(amount_to_remove / 10);
            }
            else
            {
                amount_to_remove /= 10;
            }

            for(int i = 0; i < amount_to_remove; i++)
            {
                removeRandom(waiting_tasks);
                tasks_count--;
            }
        }
        else
        {
            exit(1);
        }
    }
    if(pushQueue(waiting_tasks, request_node))
    {
        tasks_count++;
        pthread_cond_signal(&queue_not_empty);
    }
    
    pthread_mutex_unlock(&lock_queue);
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    getargs(&port,&threads_number, &req_number, argc, argv);


    pthread_mutex_init(&lock_queue, NULL);
    pthread_cond_init(&requests_not_max, NULL);
    pthread_cond_init(&queue_not_empty, NULL);
    waiting_tasks = createQueue((size_t)req_number);

    for(int i = 0; i < threads_number; i++)
    {
        ThreadInfo new_thread_info = createThreadInfo(i, argv[4]);
        if(pthread_create(&(new_thread_info->thread_id), NULL, &thread_start_routine , (void*)new_thread_info) != 0)
        {
            exit(1);
        }
    } 

    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        struct timeval arrive;
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        gettimeofday(&arrive, NULL);
        Node new_node = createNode(connfd, arrive);
        initialize_task(new_node, argv[4]);
    }
    deleteQueue(waiting_tasks);
}


    


 
