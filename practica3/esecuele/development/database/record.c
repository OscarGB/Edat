#include "record.h"

struct record_ {
  int ncols;
  long next;
  void** values;

};

record_t* record_create(void** values, int ncols, long next) {
  record_t* record = NULL;

  if(!values || ncols <= 0 || next <= 0) return NULL:

  record = (record_t*)malloc(sizeof(record_t));
  if(!record) return NULL;

  record->ncols = ncols;

  record->next = next;

  record->values = values;

  return record;
}

void* record_get(record_t* record, int n) {
  if(!record || n < 0 || n >= record->ncols) return NULL;

  return record->values[n];
}

long record_next(record_t* record) {
  if(!record) return -1;

  return record->next;
}

void record_free(record_t* record) {
  int i;

  for(i = 0; i < record->ncols; i++){
  	if(record->values[i] != NULL){
  		free(record->values[i]);
  	}
  }

  free(record);
}
