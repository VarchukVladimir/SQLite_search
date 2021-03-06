/*
 * dbfunc.h
 *
 *  Created on: Feb 12, 2014
 *      Author: Volodymyr Varchuk
 */

#ifndef DBFUNC_H_
#define DBFUNC_H_

#include <sqlite3.h>
#include "strfunc.h"

#define CHECK_DB_ERROR if (result != SQLITE_OK) databaseError (db);

int create_Table (sqlite3 *, List_t *);
void databaseError(sqlite3*);
int db_open (sqlite3 **, char *);
int get_tables_names (sqlite3 *, List_t *);
int delete_table (sqlite3 *, const char *);
int get_table_struct (sqlite3 *, KeyValueList_t *, const char *);
int insertValues (sqlite3 *, IntKeyValueList_t *, KeyValueList_t *, KeyValueList_t *, const char *);
int Query_SQL (sqlite3 *, char *);
int checkTable (sqlite3 *, char *);
int checkRec_Path_Name (sqlite3 *, char *);
int delete_record_by_ID ( sqlite3 *, char *);
int add_record (sqlite3 *, KeyValueList_t *, List_t *);
int sqlite_pragma (sqlite3*, const char* );

#endif /* DBFUNC_H_ */
