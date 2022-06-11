
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
static LinkedList busy_tasks;
static ThreadInfo *thread_info;

pthread_mutex_t lock_queue, lock_list;
pthread_cond_t requests_max, queue_not_empty;

int tasks_count = 0;
int threads_number, req_number;


void getargs(int *port, int *threads_number, int *req_number, int argc, char *argv[])
{
    if (argc < 4) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *threads_number = atoi(argv[2]);
    *req_number = atoi(argv[3]);
}

void* thread_start_routine(void* thread_info)
{
    ThreadInfo curr_thread_info = (ThreadInfo)(thread_info);
    while(1)
    {
        pthread_mutex_lock(&lock_queue);
        while(waiting_tasks -> size == 0)
        {
            pthread_cond_wait(&queue_not_empty, &lock_queue);
        }
        Node head_node = popQueue(waiting_tasks);
        struct timeval temp;
        gettimeofday(&temp, NULL); 
        timersub(&(head_node->stat_req_arrival), &temp, &(head_node->stat_req_dispatch));
        pthread_mutex_unlock(&lock_queue);


        pthread_mutex_lock(&lock_list);
        curr_thread_info->request_node=head_node;
        insertLinkedList(busy_tasks, head_node);
        pthread_mutex_unlock(&lock_list);
        int conn_fd = curr_thread_info->request_node->connection_fd;
        requestHandle(conn_fd, curr_thread_info);

        close(conn_fd);
        pthread_mutex_lock(&lock_list);
        removeFromLinkedList(busy_tasks, head_node);
        tasks_count--;
        pthread_mutex_unlock(&lock_list);

    }
}

void initialize_task(Node request_node, char* sched_policy)
{
    pthread_mutex_lock(&lock_queue);
    if (tasks_count == req_number)
    {
        if(strcmp(sched_policy ,"block") == 0){            
            while(tasks_count == req_number)
            {
                pthread_cond_wait(&requests_max, &lock_queue);
            }
        }
        else if(strcmp(sched_policy,"drop_tail") == 0){
            Close (request_node->connection_fd);
            pthread_mutex_unlock(&lock_queue);
            return;
        }
        else if (strcmp(sched_policy, "drop_head") == 0){
            if(waiting_tasks->size == 0)
            {
                Close(request_node->connection_fd);
                pthread_mutex_unlock(&lock_queue);
                return;
            }
            Node request_to_be_deleted = popQueue(waiting_tasks);
            Close(request_to_be_deleted->connection_fd);
            free(request_to_be_deleted);
            tasks_count--;
            pthread_mutex_unlock(&lock_queue);
        }
        else if(!strcmp(sched_policy,"random")){
            if(waiting_tasks->size == 0)
            {
                Close(request_node->connection_fd);
                pthread_mutex_unlock(&lock_queue);
                return;
            }
            int amount_to_remove = (waiting_tasks->size * 3) / 10;
            for(int i=0; i<amount_to_remove; i++)
            {
                removeRandom(waiting_tasks);
                tasks_count--;
            }
        }
        else{
            // fprintf(stderr, "illegal usage");
            // exit(1);
        }
    }
    pushQueue(waiting_tasks, request_node);
    tasks_count++;
    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&lock_queue);
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    getargs(&port,&threads_number, &req_number, argc, argv);


    pthread_mutex_init(&lock_queue, NULL);
    pthread_mutex_init(&lock_list, NULL);
    pthread_cond_init(&requests_max, NULL);
    pthread_cond_init(&queue_not_empty, NULL);
    waiting_tasks = createQueue();
    busy_tasks = createLinkedList();

    thread_info = (ThreadInfo *)malloc(sizeof(ThreadInfo) * threads_number);
    if(thread_info == NULL)
    {
        return -1;
    }
    for(int i=0; i<threads_number; i++)
    {
        ThreadInfo new_thread_info = malloc(sizeof(*new_thread_info));
        
        new_thread_info->request_node = NULL;
        new_thread_info->thread_index = i;
        new_thread_info->thread_count = 0;
        new_thread_info->thread_static_count = 0;
        new_thread_info->thread_dynamic_count = 0;
        new_thread_info->sched_policy = argv[4];
        int rc = pthread_create(&(new_thread_info->thread_id), NULL, &thread_start_routine , (void*)new_thread_info);
        if(rc)
        {
            // thread failure
        }
        thread_info[i] = new_thread_info;
    } 

    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);


        Node new_node = (Node)malloc(sizeof(*new_node));
        new_node->connection_fd = connfd;
        gettimeofday(&new_node->stat_req_arrival, NULL); 
        if(new_node == NULL)
        {
            return -1;
        }
        new_node -> connection_fd = connfd;
        initialize_task(new_node, argv[4]);
    }

}


    


 
