#include "db.h" 

int dbcount = 0;
database db[1000];
char *dbpath ="";

void set_db_path(char *path){
	dbpath = path;
}

void rereadstr(FILE *f,char *string){
	do{
		fgets(string, 255, f);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}

int db_open(char *name){
	FILE *stream;
	char *filetmp = (char *)calloc(strlen(dbpath)+strlen(name)+1, sizeof(char));
	int rowcount=0;
	char oneline[255];
	strncpy(filetmp, dbpath, strlen(dbpath));
	strcat(filetmp, name);
	if((stream = fopen(filetmp, "rt" )) != NULL){
		dbcount++;
		db[dbcount].name = filetmp;
		/*load entries into struct*/
		while(!feof( stream )){   
			rereadstr(stream, oneline);
			sscanf(oneline, "%i#%*s#%*s", &db[dbcount].dbrow[rowcount].id);
			char cha = '#';
			char chb = '\n';
			char *p = strchr(oneline, cha);
			int p1 = oneline - p;
			if(p1<0) {
     				p1 *= -1;  
    			}
			p = strrchr(oneline, cha);
			int p2 = oneline - p;
			if(p2<0) {
     				p2 *= -1;  
    			}
			p = strchr(oneline, chb);
			int p3 = oneline - p;
			if(p3<0) {
     				p3 *= -1;  
    			}
			db[dbcount].dbrow[rowcount].location = (char *)calloc(p2-p1, sizeof(char));
			strncat(db[dbcount].dbrow[rowcount].location, oneline+p1+1, p2-p1-1);
			db[dbcount].dbrow[rowcount].name = (char *)calloc(p3-p2, sizeof(char));
			strncat(db[dbcount].dbrow[rowcount].name, oneline+p2+1, p3-p2-1);
			printf("row id : %i \nname : %s\nlocation : %s\n", db[dbcount].dbrow[rowcount].id, db[dbcount].dbrow[rowcount].name, db[dbcount].dbrow[rowcount].location);
			db[dbcount].rowcount = rowcount;
			rowcount++;
		}
		printf("database loaded : %s\n", filetmp);
		fclose(stream);
		free(filetmp);
		return dbcount;
	}else{
		printf("database does not exist: %s\n", filetmp);
		free(filetmp);
		return 0;
	}
}

char * db_get_row_item_by_id(int idb, int irow, int col){
	for(int i=0;i<db[idb].rowcount;i++){
		if(db[idb].dbrow[i].id==irow){
			if(col == 1){
				return db[idb].dbrow[i].location;
				break;
			}
			if(col == 2){
				return db[idb].dbrow[i].name;
				break;
			}	
		}
	}
	return NULL;
}

char * db_get_row_item_by_slot(int idb, int slot, int col){
	if(col == 1){
		return db[idb].dbrow[slot].location;
	}else if(col == 2){
		return db[idb].dbrow[slot].name;
	}else{	
		return NULL;
	}
}

int  db_get_row_count(int idb){
	return db[idb].rowcount;
}

int  db_get_row_id_from_slot(int idb, int slot){
	return db[idb].dbrow[slot].id;
}
void db_new_row(int idb){
	db[idb].rowcount++;
	
}

void db_insert_row_item(int idb, int irow, int col, char *dat){
	for(int i=0;i<db[idb].rowcount;i++){
		if(db[idb].dbrow[i].id==irow){
			if(col == 1){
				db[idb].dbrow[i].location = dat;
				break;
			}
			if(col == 2){
				db[idb].dbrow[i].name = dat;
				break;
			}	
		}
	}
}
