#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "../type/type.h"

struct table_ {

  int ncols; /*Numero de columnas*/
  type_t* types; /*Array de tipos*/
  FILE* file; /*Fichero de la tabla*/
  long first_pos; /*Primera posicion*/
  long last_pos; /*Ultima posicion*/

};

void table_create(char* path, int ncols, type_t* types) {

  FILE *f = NULL;

  if(!path || ncols < 1 || !types){
    return;
  }

  f = fopen(path, "w+");
  if(!f){
    return;
  }

  fwrite(&ncols, sizeof(int), 1, f);
  fwrite(types, sizeof(type_t), ncols, f);

  fclose(f);

}

table_t* table_open(char* path) {

  table_t* table = NULL;

  if(!path){
    return NULL;
  }

  table = (table_t*)malloc(sizeof(table_t));
  if(!table) return NULL;

  table->file = fopen(path, "r+");
  if(!table->file){
    free(table);
    return NULL;
  }


  fread(&(table->ncols), sizeof(int), 1, table->file);

  table->types = (type_t*)malloc(sizeof(type_t)*table->ncols);
  if(!table->types){
    fclose(table->file);
    free(table);
    return NULL;
  }

  fread(table->types, sizeof(type_t), table->ncols, table->file);

  table->first_pos = ftell(table->file);
  /*fseek(table->file, 0, SEEK_END);*/
  table->last_pos = ftell(table->file);

  return table;

}

void table_close(table_t* table) {

  if(!table){
    return;
  }

  fclose(table->file);
  free(table->types);
  free(table);

  return;

}

int table_ncols(table_t* table) {

  if(!table){
    return -1;
  }

  return table->ncols;

}

type_t* table_types(table_t* table) {

  if(!table){
    return NULL;
  }

  return table->types;

}

long table_first_pos(table_t* table) {

  if(!table){
    return -1;
  }

  return table->first_pos;

}

long table_last_pos(table_t* table) {

  if(!table){
    return -1;
  }

  fseek(table->file, 0, SEEK_END);
  table->last_pos = ftell(table->file);
  return table->last_pos;

}

record_t* table_read_record(table_t* table, long pos) {

  record_t* record = NULL;
  void** values;
  int i;
  size_t size;

  if(!table || pos >= table->last_pos || pos < 0){
    return NULL;
  }

  values = (void **)malloc(sizeof(void*)*table->ncols);
  if(!values){
    return NULL;
  }

  fseek(table->file, pos, SEEK_SET);

  for(i = 0; i < table->ncols; i++){
    fread(&size, sizeof(type_t), 1, table->file);
    values[i] = (void *)malloc(size);
    fread(values[i], size, 1, table->file);
  }

  record = record_create(values, table->ncols, ftell(table->file));
  if(!record){
    for(i = 0; i < table->ncols; i++){
      free(values[i]);
    }
    free(values);
    return NULL;
  }

  return record;

}

void table_insert_record(table_t* table, void** values) {
  
  int i;
  size_t size;

  if(!table || !values){
    return;
  }

  fseek(table->file, 0L, SEEK_END);

  for(i = 0; i < table->ncols; i++){
    size = value_length(table->types[i], values[i]);
    fwrite(&size, sizeof(size_t), 1, table->file);
    fwrite(values[i], size, 1, table->file);
  }

  table->last_pos = ftell(table->file);
  return;

}
