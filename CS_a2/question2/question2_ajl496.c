#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct arguments {
  int *right_seq_count;
  int *current_iteration;
  int *buffer;
  int *seq_count;
  int value;
  pthread_mutex_t *mutex;
  bool *cont_running;
};

void *do_work( void *arg );
void *do_work2( void *arg );
void *checkForSeq( void *arg );

int main() {

  pthread_t worker_thread[6];
  pthread_mutex_t lock1;
  pthread_mutex_t lock2;
  pthread_mutex_init( &lock1, NULL );
  pthread_mutex_init( &lock2, NULL );
  struct arguments *arg[6];

  int right_seq_count1 = 0;
  int right_seq_count2 = 0;
  int seq_count1 = 0;
  int seq_count2 = 0;
  int buffer1[3];
  int buffer2[3];
  int current_iteration1 = 0;
  int current_iteration2 = 0;
  bool cont_running = true;

  for( int i = 0; i < 3; i = i + 1 )
    {
      // Build argument to threads
      arg[i] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
      arg[i]->value=i+1;
      arg[i]->right_seq_count = &right_seq_count1;
      arg[i]->seq_count = &seq_count1;
      arg[i]->mutex = &lock1;
      arg[i]->buffer = buffer1;
      arg[i]->current_iteration = &current_iteration1;
      arg[i]->cont_running = &cont_running;

      // Create thread1
      if ( pthread_create( &worker_thread[i+1], NULL,
                           do_work, ( void * )arg[i])) {
        fprintf( stderr,"Error while creating thread\n" );
        exit( 1 );
      }
    }

  for( int i = 3; i < 6; i = i + 1 )
    {
      // Build argument to threads
      arg[i] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
      arg[i]->value=i+1;
      arg[i]->right_seq_count = &right_seq_count2;
      arg[i]->seq_count = &seq_count2;
      arg[i]->mutex = &lock2;
      arg[i]->buffer = buffer2;
      arg[i]->current_iteration = &current_iteration2;
      arg[i]->cont_running = &cont_running;

      // Create thread1
      if ( pthread_create( &worker_thread[i+1], NULL,
                           do_work2, ( void * )arg[i])) {
        fprintf( stderr,"Error while creating thread\n" );
        exit( 1 );
      }
    }

  for( int i = 0; i < 6; i = i + 1 )
    {
      // Join with thread
      if ( pthread_join( worker_thread[i+1], NULL ) ) {
        fprintf( stderr,"Error while joining with child thread\n" );
        exit( 1 );
      }
    }

  printf( "Team %d won!\n", ( right_seq_count1 < right_seq_count2 ) + 1 );
  printf( "Total sequences generated team1: %d\n", seq_count1 );
  printf( "Total sequences generated team2: %d\n", seq_count2 );
  printf( "Number of correct sequences team1: %d\n", right_seq_count1 );
  printf( "Number of correct sequences team2: %d\n", right_seq_count2 );

  exit( 0 );

  return 0;
}

void *do_work( void *arg )
  {
    struct arguments *argument;
    argument = ( struct arguments *) arg;
    const int LAST_NUM = 3;

    int *buffer;
    buffer = argument->buffer;
    pthread_mutex_t *mutex = argument->mutex;
    int *right_seq_count = argument->right_seq_count;
    int *seq_count = argument->seq_count;
    int *current_iteration = argument->current_iteration;
    int value = argument->value;
    bool *cont_running = argument->cont_running;

    while( *cont_running )
      {
        pthread_mutex_lock( mutex );

        printf( "My id: %d\n", value );

        buffer[*current_iteration] = value;

        *current_iteration = *current_iteration + 1;

        if( *current_iteration == LAST_NUM )
          {
            *seq_count = *seq_count + 1;
            checkForSeq( arg );
            *current_iteration = 0;
          }

        pthread_mutex_unlock( mutex );

        if( *right_seq_count == 10 )
          *cont_running = false;

        usleep(500000);
      }
    return NULL;
  }


void *do_work2( void *arg )
  {
    struct arguments *argument;
    argument = ( struct arguments *) arg;
    const int LAST_NUM = 3;

    int *buffer2;
    buffer2 = argument->buffer;
    pthread_mutex_t *mutex2 = argument->mutex;
    int *right_seq_count2 = argument->right_seq_count;
    int *seq_count2 = argument->seq_count;
    int *current_iteration2 = argument->current_iteration;
    int value = argument->value;
    bool *cont_running = argument->cont_running;

    while( *cont_running )
      {
        pthread_mutex_lock( mutex2 );

        printf( "My id: %d\n", value );

        buffer2[*current_iteration2] = value;

        *current_iteration2 = *current_iteration2 + 1;

        if( *current_iteration2 == LAST_NUM )
          {
            *seq_count2 = *seq_count2 + 1;
            checkForSeq( arg );
            *current_iteration2 = 0;
          }

        pthread_mutex_unlock( mutex2 );

        if( *right_seq_count2 == 10 )
          *cont_running = false;

        usleep(500000);
      }
    return NULL;
  }

void *checkForSeq( void *arg )
  {
    struct arguments *argument;
    argument = ( struct arguments *) arg;
    int *buffer;
    buffer = argument->buffer;
    int *right_seq_count = argument->right_seq_count;

    if( ( buffer[0] == 1 && buffer[1] == 2 && buffer[2] == 3 ) ||
      ( buffer[0] == 4 && buffer[1] == 5 && buffer[2] == 6 ) )
      {
        printf("%d%d%d\n", buffer[0], buffer[1], buffer[2] );
        *right_seq_count = *right_seq_count + 1;
      }

    return NULL;
  }
