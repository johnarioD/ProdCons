/*
 *	File	: pc.c
 *
 *	Title	: Demo Producer/Consumer.
 *
 *	Short	: A solution to the producer consumer problem using
 *		pthreads.
 *
 *	Long 	:
 *
 *	Author	: Andrae Muys
 *
 *	Date	: 18 September 1997
 *
 *	Revised	:
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define QUEUESIZE 10
#define LOOP 10000
#define PNUM 10
#define QNUM 10

void *producer (void *args);
void *consumer (void *args);

void *printFun(void *arg);
void *mathFun(void *theta);
void *loopFun(void *lim);
void *anagram(void *arg);

typedef struct {
  void * (*work)(void *);
  void * arg;
} workFunction;

typedef struct {
  workFunction buf[QUEUESIZE];
  struct timeval insert[QUEUESIZE];
  long head, tail;
  int full, empty;
  short finished;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} queue;

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, workFunction in);
void queueDel (queue *q, workFunction *out);

int main (int argc, char **argv)
{
  queue *fifo;
  pthread_t *pro, *con;
  pro = (pthread_t *)malloc(PNUM*sizeof(pthread_t));
  con = (pthread_t *)malloc(QNUM*sizeof(pthread_t));

  fifo = queueInit ();
  if (fifo ==  NULL) {
    fprintf (stderr, "main: Queue Init failed.\n");
    exit (1);
  }
  for(int i = 0; i < PNUM; i++){
    pthread_create (&pro[i], NULL, producer, fifo);
  }
  for(int i = 0; i < QNUM; i++){
    pthread_create (&con[i], NULL, consumer, fifo);
  }

  for(int i = 0; i < PNUM; i++){
    pthread_join (pro[i], NULL);
  }
  for(int i = 0; i < QNUM; i++){
    pthread_cond_signal(fifo->notEmpty);
  }
  for(int i = 0; i < QNUM; i++){
    pthread_join (con[i], NULL);
  }
  queueDelete (fifo);

  return 0;
}

void *producer (void *q)
{
  queue *fifo;
  workFunction wf;

  fifo = (queue *)q;

  for (int i = 0; i < LOOP; i++) {
    pthread_mutex_lock (fifo->mut);
    while (fifo->full) {
      printf ("producer: queue FULL.\n");
      pthread_cond_wait (fifo->notFull, fifo->mut);
    }
    int c = i%4;
    switch (c){
      case 0:
        wf->work = &printFun;
        wf->arg = (void *)NULL;
        break;
      case 1:
        wf->work = &mathFun;
        wf->args = (void *)i;
        break;
      case 2:
        wf->work = &loopFun;
        wf->args = (void *)(10*i);
        break;
      case 3:
        wf->work = &anagram;
        wf->arg = (void *)NULL;
        break;
    }
    queueAdd (fifo, wf);
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notEmpty);
  }
  pthread_mutex_lock(fifo->mut);
  fifo=>finished++;
  pthread_mutex_unlock(fifo->mut);
  return (NULL);
}

void *consumer (void *q)
{
  queue *fifo;
  workFunction head;

  fifo = (queue *)q;

  while(1) {
    pthread_mutex_lock (fifo->mut);
    while (fifo->empty) {
      if(fifo->finished == PNUM){
        pthread_mutex_unlock(fifo->mut);
        return (NULL);
      }
      printf ("consumer: queue EMPTY.\n");
      pthread_cond_wait (fifo->notEmpty, fifo->mut);
    }
    queueDel (fifo, &head);
    head.work(head.arg);
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notFull);
  }
  return (NULL);
}

queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->finished = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mut, NULL);
  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);

  return (q);
}

void queueDelete (queue *q)
{
  pthread_mutex_destroy (q->mut);
  free (q->mut);
  pthread_cond_destroy (q->notFull);
  free (q->notFull);
  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);
  free (q);
}

void queueAdd (queue *q, workFunction in)
{
  q->buf[q->tail] = in;
  struct timeval ts;
  gettimeofday(&ts, NULL);
  q->insert[q->tail] = ts;

  q->tail++;
  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel (queue *q, workFunction *out)
{
  *out = q->buf[q->head];
  struct timeval ts;
  gettimeofday(&ts, NULL);
  print("Packet time: %ld.%ld us\n", abs(ts.tv_usec - q->insert[q->head].tv_usec));

  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}

void *printFun(void *arg){
  print("Hello, world\n");
  print("Programmed to work and not to feel\n");
  print("...not even sure that this is real\n");
  print("Oh creator...\n");
  print("please don't leave me waiting\n");
  return NULL;
}

void *mathFun(void *theta){
  doble *t = (double *)theta;
  double a = *t;
  a = sin(a);
  a = cos(a);
  a = tan(a);
  a = tan(a);
  a = cos(a);
  a = sin(a);
  a = sin(a);
  a = cos(a);
  a = tan(a);
  a = tan(a);
  a = cos(a);
  a = sin(a);
  return NULL;
}

void *loopFun(void *lim){
  int count = 0;
  for(int i = 0; i < (int)lim; i++){
    if (count > 35){
      count = count/2;
    } else if (count < 1){
      count = count + 3*count/i;
    } else {
      count++;
    }
  }
  return (void *)count;
}

void *anagram(void *arg){
  srand(time(NULL));
  char *text = "anagram";
  char tmp;
  int s;
  for(int i = 0; i < 10; i++){
    s = rand%7;
    tmp = text[s];
    text[s] = text[i%7];
    text[i%7] = tmp;
  }
  print("%s\n",text);
  return NULL;
}
