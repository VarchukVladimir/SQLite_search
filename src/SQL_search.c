/*
 * SQL_search.c
 *
 *  Created on: Feb 4, 2014
 *      Author: Volodymyr Varchuk
 */
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "opts.h"
#include "loadsets.h"
#include "strfunc.h"
#include "dbfunc.h"
#include "filefunc.h"
#include "env.h"


#define USAGE "USAGE SQL_search \n\
--add -a \n\
\tadd record to DB \n\n\
--find -f <key1:val1> <key2:val2> \n\
\tfind records in DB with filters: key - metatag name, val - filter \n\n\
--delete - d <key1:val1> <key2:val2>\n\
\tdelete records with filters: key - metatag name, val - filter from DB\n\n\
--struct -s\n\
\tcreate srtruct\n\n\
--list -l\n\
\tload list of object files, columns, key:vals from stdin\n\n\
--match -m\n\
\tmatch result\n\n\
\n\
Examples:\n\
./SQL_search --find <key1:val1> <key2:val2> --list\n\
\tdo query and filtering result by metatags name and values key1=val1 key2=val2\n\
\tin result show only columns which specified in stdin\n\
\n\
\n"

extern char ** environ;

int do_struct_db ( sqlite3 *db, List_t *list )
{
	int result = create_Table_FTS( db, list);
	CHECK_DB_ERROR;
	return 0;
}

int delete_record (sqlite3 *db, Options_t *opts)
{
	int i = 0;
	char * fileName = NULL;

	if (opts->pObject_path == NULL)
	{
		if (getenv( PATH_INFO_NAME ) != NULL)
			delete_record_by_ID ( db, getenv (PATH_INFO_NAME) );
		else
			return -1;
	}
	else
		delete_record_by_ID ( db, opts->pObject_path );
	return 0;
}

int addRecord (sqlite3 *db, KeyValueList_t *envKVList, List_t *fieldList, Options_t *opts)
{
	int reccount = checkRec_Path_Name (db, DB_TABLE_NAME_FTS);
//	printf (" %d\n", reccount);
	if ( reccount != 0 )
	{
		// удалить или перезаписать существующую запись
		if (opts->iOwerWrite == 1)
		{
			if (delete_record_by_ID (db, getenv ( PATH_INFO_NAME )) != SQLITE_OK)
				return -1;
		}
		else
		{
			printf ("Existed record use option -o or --overwrite to overwrite this record\n");
			return -1;
		}
	}
	add_record( db, envKVList, fieldList );
	return 0;
}

int find_ (sqlite3 *db, List_t *fieldList, Options_t *opts)
{
	KeyValueList_t tableStruct, *pTableStruct = &tableStruct;
	initKVList(pTableStruct, 10);
	get_table_struct( db,pTableStruct, DB_TABLE_NAME_FTS);
	char *SQL_ = "SELECT %s FROM %s WHERE (%s)"; // field list, table name, where

	char *SQL_fields = NULL;
	char *SQL_where = NULL;
	char *SQL = NULL;
	int SQL_size = 0;
	int i = 0;
	int result = 0;

	if (opts->iList == 1)
		SQL_fields = getInsert_Values_Str( pTableStruct, fieldList, 1 );
	else
	{
		SQL_fields = (char *) malloc (sizeof ( char) * strlen (PATH_INFO_NAME));
		sprintf( SQL_fields, "%s", PATH_INFO_NAME );
	}


	printf ("fields %s \n", SQL_fields);
	SQL_where = getWHERE_condition(opts);

	printf ("where %s \n", SQL_where);

	SQL_size = strlen (SQL_) + strlen (SQL_fields ) + strlen ( DB_TABLE_NAME_FTS ) + (SQL_where == NULL ? 1 :strlen (SQL_where)) + 1;
	SQL = (char *) malloc ( sizeof (char) *SQL_size );

	sprintf (SQL, SQL_, SQL_fields, DB_TABLE_NAME_FTS, ( SQL_where == NULL ? "" : SQL_where));
	printf ("SQL = %s \n", SQL);

	result = Query_SQL( db, SQL );
	CHECK_DB_ERROR;

	free (SQL);
	free (SQL_fields);
	free (SQL_where);

	return 0;
}

void print_args (int argc, char ** argv)
{
	int i = 0;
	for ( i = 0; i < argc; i++)
		printf ("%d. %s \n", i, argv[i]);
}

int main (int argc, char ** argv)
{

	Options_t opts;
	int i = 0;
	int result = 0;

	sqlite3 *db;

	KeyValueList_t keyvallist, envList;
	List_t list;
	char *load_db_path = NULL;
	char *save_db_path = NULL;
	char *db_dir = NULL;
	char *db_file = DB_FILE;
	int iSQLite_result = 0;
	if ((getenv("SERVER_SOFTWARE") != NULL ) && (strcasecmp( getenv("SERVER_SOFTWARE"),  "zerocloud" )==0))
	{
		load_db_path =  strdup( LOAD_ZEROCOLUD);
		save_db_path = strdup(SAVE_ZEROCOLUD);
		db_dir = strdup(DB_DIR_ZEROCLOUD);
	}
	else
	{
		load_db_path = strdup(LOAD_LOCAL);
		save_db_path = strdup(SAVE_LOCAL);
		db_dir = strdup(DB_DIR_LOCAL);
	}
	int listcount = 0;
	initKVList ( &keyvallist, 10);
	initKVList( &envList, 10 );
	initList ( &list, 10);
	getEnvList( &envList );


	newbufferedunpack(db_dir, load_db_path);


	//add check if the db file was load from channel FIXME
	db_open (&db, gen_db_file_name( db_dir, db_file ) );
	//printf ( "rows in table %d \n", do_Query_SQL_row_count (db, "SELECT * FROM metatags_FTS"));

	if ( getopts (argc, argv, &opts, &keyvallist, &list) != 0)
	{
		printf ("Wrong command line parameter\n");
		printf (USAGE);
		return -1;
	}

	if (opts.tMode == none)
	{
		printf ("Not specified operation\n");
		printf (USAGE);
		return -1;
	}

	int ExistTable = 0;

	if (check_str_in_list ( &list, PATH_INFO_NAME ) == 0)
	{
		printf ("Error can`t find environment variable %s \n", PATH_INFO_NAME);
		return -1;
	}

	if (( ExistTable = checkTable(db, DB_TABLE_NAME_FTS)) == 1 || opts.tMode == struct_create )
	{
		do_struct_db( db, &list );
	}

	if ( opts.tMode == add )
	{
		addRecord ( db, &envList, &list, &opts );
	}

	if (opts.tMode == find)
	{
		find_(db, &list, &opts);
	}

	if (opts.tMode == delete)
	{
		delete_record( db, &opts );
	}

	if ( opts.tMode == print )
	{
		printf_db_struct( db );
	}

	//Query_SQL (db, "SELECT * from metatags_FTS where metatags_FTS MATCH 'git'");

	printf ( "rows in table metatags_FTS %d \n", do_Query_SQL_row_count (db, "SELECT * FROM metatags_FTS"));

	result = sqlite3_close(db);
	CHECK_DB_ERROR;
	newbufferedpack(save_db_path, db_dir);
	return 0;
}

