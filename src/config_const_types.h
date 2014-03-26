/*
 * types.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Volodymyr Varchuk
 */

#ifndef OPTS_TYPES_H_
#define OPTS_TYPES_H_
/////////////////////////////////////////////////////////////////
// OPTIONS
/////////////////////////////////////////////////////////////////
typedef enum {add, delete, find, struct_create, none, print} Operation_t;
typedef struct {
	Operation_t tMode;
	char *pObject_path;
	int iList;
	int iMatch;
	int iOwerWrite;
	char **pKey;
	char **pVal;
	int iCount;
} Options_t;

/////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////
// STRING && LISTs
/////////////////////////////////////////////////////////////////
typedef struct {
	char **list;
	int count;
	int max_count;
} List_t;

typedef struct {
	char **pKey;
	char **pVal;
	int count;
	int max_count;
} KeyValueList_t;

typedef struct {
	char **pKey;
	int *pVal;
	int count;
	int max_count;
} IntKeyValueList_t;

/////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////
// DB
/////////////////////////////////////////////////////////////////
#define LOAD_LOCAL "/home/volodymyr/workspace/SQLite_search/load_save/load"
#define SAVE_LOCAL "/home/volodymyr/workspace/SQLite_search/load_save/save"
#define LOAD_ZEROCOLUD "/dev/input"
#define SAVE_ZEROCOLUD "/dev/output"
#define DB_DIR_ZEROCLOUD "/db"
#define DB_DIR_LOCAL "/home/volodymyr/workspace/SQLite_search/load_save/db"
#define DB_FILE "metatags.db"
#define DB_TABLE_NAME_FTS "metatags_FTS"
#define DB_TABLE_NAME "metatags_regular"
#define CONTETN_LENGTH_NAME "content_length"
/////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////
// ENV
/////////////////////////////////////////////////////////////////

#define PATH_INFO_NAME "PATH_INFO"
/////////////////////////////////////////////////////////////////



#endif /* OPTS_TYPES_H_ */
