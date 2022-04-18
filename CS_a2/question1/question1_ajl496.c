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
};

void *do_work( void *arg );
void *checkForSeq( void *arg );
int main( int argc, char **argv ) {

  pthread_t worker_thread1;
  pthread_t worker_thread2;
  pthread_t worker_thread3;
  pthread_mutex_t lock;
  pthread_mutex_init( &lock, NULL );
  struct arguments *arg[3];

  int right_seq_count = 0;
  int seq_count = 0;
  int buffer[3];
  int current_iteration = 0;

  // Build argument to threads
  arg[0] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
  arg[0]->value=1;
  arg[0]->right_seq_count = &right_seq_count;
  arg[0]->seq_count = &seq_count;
  arg[0]->mutex = &lock;
  arg[0]->buffer = buffer;
  arg[0]->current_iteration = &current_iteration;

  arg[1] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
  arg[1]->value=2;
  arg[1]->right_seq_count = &right_seq_count;
  arg[1]->seq_count = &seq_count;
  arg[1]->mutex = &lock;
  arg[1]->buffer = buffer;
  arg[1]->current_iteration = &current_iteration;

  arg[2] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
  arg[2]->value=3;
  arg[2]->right_seq_count = &right_seq_count;
  arg[2]->seq_count = &seq_count;
  arg[2]->mutex = &lock;
  arg[2]->buffer = buffer;
  arg[2]->current_iteration = &current_iteration;

  // Create thread1
  if ( pthread_create( &worker_thread1, NULL,
                     do_work, ( void * )arg[0])) {
    fprintf( stderr,"Error while creating thread\n" );
    exit( 1 );
  }

  // Create thread2
  if ( pthread_create( &worker_thread2, NULL,
                     do_work, ( void * )arg[1] ) ) {
    fprintf( stderr,"Error while creating thread\n" );
    exit( 1 );
  }

// Create thread3
  if ( pthread_create( &worker_thread3, NULL,
                     do_work, ( void * )arg[2] ) ) {
    fprintf( stderr,"Error while creating thread\n" );
    exit( 1 );
  }


  // Join with thread
  if ( pthread_join( worker_thread1, NULL ) ) {
    fprintf( stderr,"Error while joining with child thread\n" );
    exit( 1 );
  }

  if ( pthread_join( worker_thread2, NULL ) ) {
    fprintf( stderr,"Error while joining with child thread\n" );
    exit( 1 );
  }

  if ( pthread_join( worker_thread3, NULL ) ) {
    fprintf( stderr,"Error while joining with child thread\n" );
    exit( 1 );
  }

  printf( "Total sequences generated: %d\n", seq_count );
  printf( "Number of correct sequences: %d\n", right_seq_count );

  exit( 0 );

  return 0;
}

void *do_work( void *arg ) {

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


  while( *right_seq_count < 10 )
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

    if( buffer[0] == 1 && buffer[1] == 2 && buffer[2] == 3 )
      {
        printf("123\n");
        *right_seq_count = *right_seq_count + 1;
      }

    return NULL;
  }
