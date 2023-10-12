#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

// TODO: Add more fields to this struct
struct job
{
  int id;
  int arrival;
  int length;
  struct job *next;
  int startTime;
  int endTime;
};

/*** Globals ***/
int seed = 100;

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length)
{
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job *)malloc(sizeof(struct job));

  // tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;

  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty
  if (head == NULL)
  {
    head = tmp;
    return;
  }

  struct job *prev = head;

  // Find end of list
  while (prev->next != NULL)
  {
    prev = prev->next;
  }

  // Add job to end of list
  prev->next = tmp;
  return;
}

/*Function to read in the workload file and create job list*/
void read_workload_file(char *filename)
{
  int id = 0;
  FILE *fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL,
       *arrival = NULL,
       *length = NULL;

  struct job **head_ptr = malloc(sizeof(struct job *));

  if ((fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1)
  {
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

struct job *sorted_insert(struct job *head, struct job *new_job)
{
  if (!head || new_job->arrival < head->arrival ||
      (new_job->arrival == head->arrival && new_job->id < head->id))
  {
    new_job->next = head;
    return new_job;
  }

  struct job *current = head;
  while (current->next && (new_job->arrival > current->next->arrival ||
                           (new_job->arrival == current->next->arrival && new_job->id > current->next->id)))
  {
    current = current->next;
  }
  new_job->next = current->next;
  current->next = new_job;

  return head;
}

struct job *sort_jobs(struct job *head)
{
  struct job *sorted = NULL;
  struct job *current = head;
  struct job *next;

  while (current)
  {
    next = current->next;
    sorted = sorted_insert(sorted, current);
    current = next;
  }

  return sorted;
}

void policy_FIFO(struct job *head)
{
  // TODO: Fill this in
  // Sort the linked list in order from shortest arrival to farthest arrival time
  // Merge sort? after the list is sorted, print the list with how the output desires

  head = sort_jobs(head);

  int time = 0;
  struct job *current_job = head;

  printf("Execution trace with FIFO:\n\n");

  while (current_job)
  {
    if (current_job->arrival > time)
    {
      time = current_job->arrival;
    }
    printf("    t=%d: [Job %d] arrived at [%d], ran for [%d]\n",
           time, current_job->id, current_job->arrival, current_job->length);
    time += current_job->length;
    current_job = current_job->next;
  }

  printf("\nEnd of execution with FIFO.\n");

  return;
}

void analyze_FIFO(struct job *head)
{
  // TODO: Fill this in
  struct job *current_job = head;
  int time = 0; // this will track the current time of the scheduler
  int total_turnaround_time = 0;
  int total_waiting_time = 0;
  int total_response_time = 0;
  int num_jobs = 0;

  // printf("Begin analyzing FIFO:\n");

  while (current_job != NULL)
  {
    // If the job's arrival time is in the future, fast-forward the time
    if (current_job->arrival > time)
    {
      time = current_job->arrival;
    }

    int job_response_time = time - current_job->arrival;               // response time for this job
    int job_turnaround_time = job_response_time + current_job->length; // turnaround time for this job
    int job_waiting_time = job_response_time;                          // waiting time for this job is equal to the response time in FIFO

    printf("    Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
           current_job->id, job_response_time, job_turnaround_time, job_waiting_time);

    total_response_time += job_response_time;
    total_waiting_time += job_waiting_time;
    total_turnaround_time += job_turnaround_time;

    time += current_job->length;     // increase time by the job's length
    current_job = current_job->next; // go to the next job in the list
    num_jobs++;                      // increase the job count
  }

  float average_response_time = (float)total_response_time / num_jobs;
  float average_turnaround_time = (float)total_turnaround_time / num_jobs;
  float average_waiting_time = (float)total_waiting_time / num_jobs;

  printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", average_response_time, average_turnaround_time, average_waiting_time);
}

void swapJobs(struct job *job1, struct job *job2)
{
  int temp_id = job1->id, temp_arrival = job1->arrival, temp_length = job1->length;
  job1->id = job2->id;
  job1->arrival = job2->arrival;
  job1->length = job2->length;
  job2->id = temp_id;
  job2->arrival = temp_arrival;
  job2->length = temp_length;
}

void sortJobsByLength(struct job *head)
{
  int swapped;
  struct job *ptr1;
  struct job *lptr = NULL;

  if (head == NULL)
    return;

  do
  {
    swapped = 0;
    ptr1 = head;

    while (ptr1->next != lptr)
    {
      if (ptr1->length > ptr1->next->length)
      {
        swapJobs(ptr1, ptr1->next);
        swapped = 1;
      }
      ptr1 = ptr1->next;
    }
    lptr = ptr1;
  } while (swapped);
}

void policy_SJF(struct job *head)
{
  struct job *curr = head;
  int time = 0;

  sortJobsByLength(head);

  while (curr != NULL)
  {
    if (curr->arrival > time)
    {
      time = curr->arrival;
    }

    curr->startTime = time;
    curr->endTime = time + curr->length;

    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n",
           time, curr->id, curr->arrival, curr->length);

    time = curr->endTime;
    curr = curr->next;
  }

  printf("End of execution with SJF.\n");
}

void analyze_SJF(struct job *head)
{
  int count = 0;
  struct job *curr = head;
  struct job **tmpArr = (struct job *)malloc(sizeof(struct job *) * count);

  while (curr != NULL)
  {
    tmpArr[count++] = curr;
    curr = curr->next;
  }

  int i = 0;
  while (i < count - 1)
  {
    int j = 0;
    while (j < count - i - 1)
    {
      if (tmpArr[j]->id > tmpArr[j + 1]->id)
      {
        struct job *temp = tmpArr[j];
        tmpArr[j] = tmpArr[j + 1];
        tmpArr[j + 1] = temp;
      }
      j++;
    }
    i++;
  }

  for (int i = 0; i < count; i++)
  {
    struct job *job = tmpArr[i];

    int response_time = job->startTime - job->arrival;
    int turnaround_time = job->endTime - job->arrival;
    int wait_time = response_time;
    printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
           job->id, response_time, turnaround_time, wait_time);
  }

  int total_response_time = 0.0;
  int total_turnaround_time = 0.0;
  int total_wait_time = 0.0;

  int j = 0;
  while (j < count)
  {
    struct job *job = tmpArr[j];

    total_response_time += (job->startTime - job->arrival);
    total_turnaround_time += (job->endTime - job->arrival);
    total_wait_time += (job->startTime - job->arrival);

    j++;
  }

  double avg_response_time = total_response_time / count;
  double avg_turnaround_time = total_turnaround_time / count;
  double avg_wait_time = total_wait_time / count;

  printf("Average -- Response: %.2lf  Turnaround %.2lf  Wait %.2lf\n",
         avg_response_time, avg_turnaround_time, avg_wait_time);

  free(tmpArr);
}

int main(int argc, char **argv)
{

  if (argc < 4)
  {
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

  if (strcmp(policy, "FIFO") == 0)
  {
    policy_FIFO(head);
    if (analysis)
    {
      printf("\nBegin analyzing FIFO:\n\n");
      analyze_FIFO(head);
      printf("\nEnd analyzing FIFO.\n");
    }

    exit(EXIT_SUCCESS);
  }

  if (strcmp(policy, "SJF") == 0)
  {
    policy_SJF(head);
    if (analysis)
    {
      printf("\nBegin analyzing SJF:\n");
      analyze_SJF(head);
      printf("End analyzing SJF:\n");
    }
    exit(EXIT_SUCCESS);
  }

  // TODO: Add other policies

  exit(EXIT_SUCCESS);
}
