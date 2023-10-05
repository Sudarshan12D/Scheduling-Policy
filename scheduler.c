#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>



// TODO: Add more fields to this struct
struct job {
    int id;
    int arrival;
    int length;
    struct job *next;
};

/*** Globals ***/ 
int seed = 100;

//This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length){
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job*) malloc(sizeof(struct job));

  //tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;

  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty 
  if (head == NULL){
    head = tmp;
    return;
  }

  struct job *prev = head;

  //Find end of list 
  while (prev->next != NULL){
    prev = prev->next;
  }

  //Add job to end of list 
  prev->next = tmp;
  return;
}


/*Function to read in the workload file and create job list*/
void read_workload_file(char* filename) {
  int id = 0;
  FILE *fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL,
       *arrival = NULL, 
       *length = NULL;

  struct job **head_ptr = malloc(sizeof(struct job*));

  if( (fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1) {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");
       
    // Make sure neither arrival nor length are null. 
    assert(arrival != NULL && length != NULL);
        
    append(id++, atoi(arrival), atoi(length));
  }

  fclose(fp);

  // Make sure we read in at least one job
  assert(id > 0);

  return;
}


void policy_FIFO(struct job *head) {
  // TODO: Fill this in
  // Sort the linked list in order from shortest arrival to farthest arrival time
  // Merge sort? after the list is sorted, print the list with how the output desires

  struct job *current_job = head;
  int time = 0;
  printf("Execution trace with FIFO:\n\n");

  if (head == NULL)
  {
    printf("No jobs in the list.");
    return;
  }

  while (current_job != NULL)
  {
    if (current_job->arrival > time)
    {
      time = current_job->arrival;
    }

    printf("    t=%d: [Job %d] arrived at [%d], ran for [%d]\n", time, current_job->id, current_job->arrival, current_job->length);

    time += current_job->length;
    current_job = current_job->next;
  }

  printf("\nEnd of execution with FIFO.\n");
  

  

  return;
}

void analyze_FIFO(struct job *head) {
  // TODO: Fill this in
    struct job *current_job = head;
    int time = 0;  // this will track the current time of the scheduler
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    int num_jobs = 0;

    //printf("Begin analyzing FIFO:\n");

    while (current_job != NULL) {
        // If the job's arrival time is in the future, fast-forward the time
        if (current_job->arrival > time) {
            time = current_job->arrival;
        }

        int job_response_time = time - current_job->arrival;  // response time for this job
        int job_turnaround_time = job_response_time + current_job->length;  // turnaround time for this job
        int job_waiting_time = job_response_time;  // waiting time for this job is equal to the response time in FIFO

        printf("    Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", 
               current_job->id, job_response_time, job_turnaround_time, job_waiting_time);

        total_response_time += job_response_time;
        total_waiting_time += job_waiting_time;
        total_turnaround_time += job_turnaround_time;

        time += current_job->length;  // increase time by the job's length
        current_job = current_job->next;  // go to the next job in the list
        num_jobs++;  // increase the job count
    }

    float average_response_time = (float)total_response_time / num_jobs;
    float average_turnaround_time = (float)total_turnaround_time / num_jobs;
    float average_waiting_time = (float)total_waiting_time / num_jobs;

    printf("\nAverage -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", average_response_time, average_turnaround_time, average_waiting_time);
    
}

void policy_SJF(struct job *head) {
    struct job *current_job = head;
    struct job *shortest_job;
    int time = 0;  // to track the current time of the scheduler

    printf("Execution trace with SJF:\n\n");

    while (head != NULL) {
        shortest_job = NULL;
        current_job = head;

        // Find the shortest job that has arrived by now
        while (current_job != NULL) {
            if (current_job->arrival <= time &&
               (shortest_job == NULL || current_job->length < shortest_job->length ||
                (current_job->length == shortest_job->length && current_job->arrival < shortest_job->arrival))) {
                shortest_job = current_job;
            }
            current_job = current_job->next;
        }

        // If no job has arrived by now, fast forward time to the arrival of the next job
        if (shortest_job == NULL) {
            time++;
            continue;
        }

        printf("    t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, shortest_job->id, shortest_job->arrival, shortest_job->length);
        time += shortest_job->length;

        // Remove the shortest job from the list
        if (shortest_job == head) {
            head = shortest_job->next;
        } else {
            current_job = head;
            while (current_job->next != shortest_job) {
                current_job = current_job->next;
            }
            current_job->next = shortest_job->next;
        }
        free(shortest_job);
    }

    printf("\nEnd of execution with SJF.\n");
}

void analyze_SJF(struct job *head) {
  
    //TODO: Fill this in
}


int main(int argc, char **argv) {

 if (argc < 4) {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file\n", argv[0]);
		exit(EXIT_FAILURE);
  }

  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];

  // Note: we use a global variable to point to 
  // the start of a linked-list of jobs, i.e., the job list 
  read_workload_file(workload);

  if (strcmp(policy, "FIFO") == 0 ) {
    policy_FIFO(head);
    if (analysis) {
      printf("\nBegin analyzing FIFO:\n\n");
      analyze_FIFO(head);
      printf("\nEnd analyzing FIFO.\n");
    }

    exit(EXIT_SUCCESS);
  }

  if (strcmp(policy, "SJF") == 0) {
    policy_SJF(head);
    if (analysis) {
      printf("\nBegin analyzing SJF:\n\n");
      analyze_SJF(head);
      printf("\nEnd analyzing SJF:\n");
    }
    exit(EXIT_SUCCESS);

  }

  // TODO: Add other policies 

	exit(EXIT_SUCCESS);
}
