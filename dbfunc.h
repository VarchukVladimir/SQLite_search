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


#define LOAD_LOCAL "/home/volodymyr/workspace/SQLite_search/load_save/load"
#define SAVE_LOCAL "/home/volodymyr/workspace/SQLite_search/load_save/save"
#define LOAD_ZEROCOLUD "/dev/input"
#define SAVE_ZEROCOLUD "/dev/output"
#define DB_DIR_ZEROCLOUD "/db"
#define DB_DIR_LOCAL "/home/volodymyr/workspace/SQLite_search/load_save/db"
#define DB_FILE "metatags.db"
#define DB_TABLE_NAME "metatags_tab"
#define CONTETN_LENGTH_NAME "content_length"

#define CHECK_DB_ERROR if (result != SQLITE_OK) databaseError (db);

int create_Table (sqlite3 *, List_t *);
void databaseError(sqlite3*);
int db_open (const char *, sqlite3 *);
int get_tables_names (sqlite3 *, List_t *);
int delete_table (sqlite3 *, const char *);
int get_table_struct (sqlite3 *, KeyValueList_t *, const char *);
int insertValues (sqlite3 *, IntKeyValueList_t *, KeyValueList_t *, KeyValueList_t *, const char *);
int Query_SQL (sqlite3 *, char *);
int checkTable (sqlite3 *, char *);

#endif /* DBFUNC_H_ */
