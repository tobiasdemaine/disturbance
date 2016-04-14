#ifndef _DB_H_
#define _DB_H_
#include "glx_base.h"
typedef struct{
	int id;
	char *location;
	char *name;
}rows;

typedef struct{
	char *name;
	int rowcount;
	rows dbrow[3000];
}database;
void rereadstr(FILE *f,char *string);
void set_db_path(char *path);
int  db_open(char *name);
BOOL  db_save(char *name);
BOOL  create_new(char *name);
int  db_get_row_count(int idb);
int  db_get_row_id_from_slot(int idb, int irow);
char * db_get_row_item_by_id(int idb, int irow, int col);
char * db_get_row_item_by_slot(int idb, int irow, int col);
void db_delete_row(int idb, int row);
void db_create_new_row(int idb);
void db_insert_row_item(int idb, int irow, int col, char *dat);
void db_new_row(int idb);
#endif
