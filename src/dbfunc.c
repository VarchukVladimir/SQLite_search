/*
 * dbfunc.c
 *
 *  Created on: Feb 8, 2014
 *      Author: Volodymyr Varchuk
 */

#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "strfunc.h"
#include "dbfunc.h"
#include "env.h"

#define MAX_BUFF 1024

int do_Query_SQL (sqlite3 *db, char *SQL)
{
	return sqlite3_exec(db, SQL, 0, 0, 0);
}

int db_open (sqlite3 **db, char *db_file_name )
{
	int result = 0;
	sqlite3_open(db_file_name , db);
	if( SQLITE_OK != sqlite3_errcode(*db) )
	{
			databaseError(*db);
			return -1;
	}
	else
	{
		/*enable extended sqlite error code*/
		int result = sqlite3_extended_result_codes(*db, 1);
		fprintf(stderr, "sql extended result code enable, err code=%d \n", result);

		/*disable journaling*/
		if (!result){
			result = sqlite_pragma(*db, "PRAGMA journal_mode=MEMORY;" );
			fprintf( stderr, "set journal_mode pragma errcode=%d\n", result);
		}
		/*disable using of synchronisation, because it's not supported by ZRT FS*/
		if (!result){
			result = sqlite_pragma(*db, "PRAGMA synchronous=OFF;" );
			fprintf( stderr, "set syncronous pragma errcode=%d\n", result);
		}
		/*exclusive access to DB by single process*/
		if (!result){
			result = sqlite_pragma(*db, "PRAGMA locking_mode=EXCLUSIVE;" );
			fprintf( stderr, "set locking_mode pragma errcode=%d\n", result);
		}
	}
	return result;
}

int sqlite_pragma (sqlite3* db, const char* request){
    // get current database version of schema
    static sqlite3_stmt *stmt_pragma;
    int rc=0;

    if( (rc=sqlite3_prepare_v2(db, request, -1, &stmt_pragma, NULL)) == SQLITE_OK) {
        while(sqlite3_step(stmt_pragma) == SQLITE_ROW);
    }
    else{
	rc = sqlite3_errcode(db);
    }
    sqlite3_finalize(stmt_pragma);
    return rc;
}

int do_Query_SQL_row_count (sqlite3 *db, char *SQL)
{
	int reccount = 0;
	int result = 0;
	//sqlite3_exec(db, SQL, 0, 0, 0);
    sqlite3_stmt *pStmt;
    do {
    	result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);
        if( result != SQLITE_OK ){
        	CHECK_DB_ERROR;
        	return -1;
        }
        while ( (result = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
        	reccount++;
        }
        result = sqlite3_finalize(pStmt);
    }while (result==SQLITE_SCHEMA );

	return reccount;
}

int delete_record_by_ID ( sqlite3 *db, char* fileName )
{
	int result = 0;
	printf ("deleted record %s\n", fileName);
	char *SQL_delete_ = "DELETE FROM %s WHERE ID=%s";
	char *SQL_delete = NULL;
	SQL_delete = (char *) malloc ( sizeof ( char ) * ( strlen (SQL_delete_) + strlen ( DB_TABLE_NAME_FTS ) + strlen ( str_CRC32( fileName ))));
	sprintf ( SQL_delete, SQL_delete_, DB_TABLE_NAME_FTS, str_CRC32( fileName));
	result = do_Query_SQL (db, SQL_delete);
	CHECK_DB_ERROR;
	return result;
}

int add_record (sqlite3 *db, KeyValueList_t *envKVList, List_t *fieldList)
{
	char *SQL_ = "INSERT INTO %s(%s) VALUES(%s)"; // tablaname, field list, values;
	char *SQL = NULL;
	char *SQL_fields = NULL;
	char *SQL_values = NULL;
	int result = 0, i = 0, j = 0;
	KeyValueList_t tableStruct;
	List_t fieldListFinal;
	initKVList( &tableStruct, 10 );
	initList( &fieldListFinal, 10 );

	get_table_struct( db, &tableStruct, DB_TABLE_NAME_FTS );

	//
	for ( i = 0; i < fieldList->count; i++ )
		for ( j = 0; j < tableStruct.count; j++)
			if ( strcasecmp( fieldList->list[i], tableStruct.pKey[j] ) == 0 )
				addToList( fieldList->list[i], &fieldListFinal );


	SQL_fields = getInsert_Values_Str_ID ( envKVList, &fieldListFinal, 1, 1 );
	SQL_values = getInsert_Values_Str_ID ( envKVList, &fieldListFinal, 0, 1 );

	int sizeSQL = (strlen ( SQL_fields ) + strlen (SQL_values) + strlen(SQL_) + strlen (DB_TABLE_NAME_FTS) + 1 );
	SQL = (char *) malloc ( sizeof (char) * sizeSQL);
	sprintf  (SQL, SQL_, DB_TABLE_NAME_FTS, SQL_fields, SQL_values);
	result = do_Query_SQL (db, SQL);

	free (SQL);
	free (SQL_fields);
	free (SQL_values);
	if (result == SQLITE_OK)
		printf ("added record %s\n", getenv( PATH_INFO_NAME ));
	CHECK_DB_ERROR;
	return result;
}

int checkRec_Path_Name (sqlite3 *db, char *tableName)
{
	char *pathInfo = NULL;
	char *SQL_ = "SELECT * FROM %s WHERE %s"; // 1. PATH_INFO field, 2. table name, 3. filter field PATH_INFO, 4. filter val
	char *SQL_Where = NULL;
	char *SQL = NULL;
	pathInfo = getenv (PATH_INFO_NAME);
	if (pathInfo == NULL)
		return -1;
	SQL_Where = (char *) malloc ( sizeof (char ) * strlen ( PATH_INFO_NAME ) + strlen ( str_CRC32( pathInfo ) ) + 5 );
	sprintf (SQL_Where, "%s=%s", "ID", str_CRC32( pathInfo ));
	SQL = (char *) malloc ( sizeof (char) * ( strlen (SQL_) + strlen ( SQL_Where ) + 20 + strlen (tableName) )  );

	sprintf (SQL, SQL_, tableName, SQL_Where );

//	printf ("check record SQL %s\n", SQL);
	if (do_Query_SQL_row_count (db, SQL) > 0)
	{
		free( SQL_Where );
		free (SQL);
		return 1;
	}
	free (SQL);
	free( SQL_Where );
	return 0;
}

/*
 *          0 if table exist else 1
 */

int checkTable (sqlite3 *db, char *tableName)
{
	List_t listTables;
	int i = 0;
	initList( &listTables, 10 );
	get_tables_names (db, &listTables);
	for (i = 0; i < listTables.count; i++)
		if ( MY_STRNCMP(tableName, listTables.list[i]) == 0 )
		{
			return 0;
		}
	return 1;
}

int Query_SQL (sqlite3 *db, char * SQL)
{

	printf ("%s \n", SQL);

    sqlite3_stmt *pStmt;
    int rowcount = 0;
    int result;
    do {
    	result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);
        if( result!=SQLITE_OK ){
        	CHECK_DB_ERROR;
        	return result;
        }

        while ( (result = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            int  i = 0;
            rowcount++;
            for ( i = 0; i < sqlite3_column_count(pStmt); i++)
            {
            	int colbytes = sqlite3_column_bytes (pStmt, i);
            	char buff [colbytes];
            	if (sqlite3_column_type(pStmt, i) == 3)
            	{
            		if (i == (sqlite3_column_count(pStmt) - 1))
               			printf ( "%s, %s\t", sqlite3_column_name(pStmt, i), sqlite3_column_text(pStmt, i) );
            		else
            			printf ( "%s, %s\t|", sqlite3_column_name(pStmt, i), sqlite3_column_text(pStmt, i) );
            	}
            	else
            		if (i == (sqlite3_column_count(pStmt) - 1))
            			printf ( "%s, %d\t", sqlite3_column_name(pStmt, i), sqlite3_column_int(pStmt, i) );
            		else
            			printf ( "%s, %d\t|", sqlite3_column_name(pStmt, i), sqlite3_column_int(pStmt, i) );
            }
            printf ("\n");
        }
        result = sqlite3_finalize(pStmt);
    }while (result==SQLITE_SCHEMA );

    printf ("query result have %d rows\n", rowcount);
	return result;
}

int insertSQL (sqlite3 *db, const char * SQL)
{
	int result = 0;
	sqlite3_stmt *pStmt;
	do {
		result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);

		if( result!=SQLITE_OK ){
                return result;
        }
        result = sqlite3_step(pStmt);
        result = sqlite3_finalize(pStmt);
	} while (result == SQLITE_SCHEMA);
	return result;
}

int insertValues (sqlite3 *db, IntKeyValueList_t *intList, KeyValueList_t *strList, KeyValueList_t *fieldList, const char *tableName)
{
	int i = 0, j =0;
	char *SQL = NULL;
	char *SQL_ = "INSERT INTO %s(%s) VALUES(%s)"; // first string - table name, second string - inserting field name, third string "? ?" template for inserting values
	char *SQL_fields = NULL;
	char *SQL_values_template = NULL;
	int SQL_fields_size = 0;
	int SQL_values_template_size = 0;
	sqlite3_stmt *pStmt;
	int result = 0;
	if (tableName == NULL )
		return -1;
	for (i = 0; i < fieldList->count; i++)
	{
		SQL_fields_size += strlen (fieldList->pKey[i]) + 2;
	}
	SQL_fields_size++;
	SQL_values_template_size = fieldList->count * 3 + 1;
	SQL_values_template = (char *) malloc ( sizeof (char) * SQL_values_template_size);
	SQL_fields = (char *) malloc ( sizeof (char) * SQL_fields_size);
	SQL = (char *) malloc ( sizeof (char) * (strlen (SQL_) + strlen (tableName) + SQL_values_template_size + SQL_fields_size) );
	if (SQL == NULL || SQL_values_template == NULL || SQL_fields == NULL)
		return -1;
	SQL_values_template[0] = '\0';
	SQL_fields[0] = '\0';

	for (i = 0; i < fieldList->count; i++)
	{
		sprintf (SQL_values_template + strlen (SQL_values_template), "?, " );
		sprintf (SQL_fields + strlen (SQL_fields), "%s, ", fieldList->pKey[i] );
	}
	SQL_values_template[strlen (SQL_values_template) - 2] = '\0';
	SQL_fields[strlen (SQL_fields) - 2] = '\0';
	sprintf (SQL, SQL_, tableName, SQL_fields, SQL_values_template);
	printf ("%s\n", SQL);
	do {
		result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);

		if( result!=SQLITE_OK ){
                return result;
        }
		for (i = 0; i < fieldList->count; i++)
		{
			// adding strng values
			for ( j = 0; j < strList->count; j++)
			{
				if ( MY_STRNCMP( strList->pKey[j], fieldList->pKey[i]) == 0)
				{
					sqlite3_bind_text (pStmt, i + 1, strList->pVal[j], -1, SQLITE_STATIC) ;// add value
				}
			}
			// adding int values
			for ( j = 0; j < intList->count; j++)
				if ( MY_STRNCMP( intList->pKey[j], fieldList->pKey[i]) == 0)
				{
					if ( MY_STRNCMP( fieldList->pVal[j], "") == 0)
					{
						char buff_int [20];
						sprintf(buff_int, "%d", intList->pVal[j]);
						sqlite3_bind_text (pStmt, i + 1, buff_int, -1, SQLITE_STATIC) ;//	 add value
					}
					else
						sqlite3_bind_int(pStmt, i + 1, intList->pVal[j]) ;// add value
				}
		}
        result = sqlite3_step(pStmt);
        result = sqlite3_finalize(pStmt);
	} while (result == SQLITE_SCHEMA);
	return 0;
}

int get_tables_names (sqlite3 *db, List_t *list)
{
	int table_count = 0;
	int result = 0;
	char * SQL = "SELECT name FROM sqlite_master WHERE type='table'"	;
    sqlite3_stmt *pStmt;
    do {
    	result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);
        if( result!=SQLITE_OK ){
                return result;
        }

        while ((result = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
        	addToList( sqlite3_column_text (pStmt, 0), list);
        }
        result = sqlite3_finalize(pStmt);
    } while ( result == SQLITE_SCHEMA);

	return table_count;
}

int get_table_struct (sqlite3 *db, KeyValueList_t *list, const char * tableName )
{
	char *SQL_ = "SELECT * FROM %s LIMIT 1";
	char *SQL = NULL;
    sqlite3_stmt *pStmt;
    int result = 0;

    SQL = (char *) malloc (sizeof (char) * (strlen (tableName) + strlen (SQL_) + 2));
    sprintf (SQL, SQL_, tableName);
    do {
    	result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);


    	if( result!=SQLITE_OK ){
                return result;
        }

        result = sqlite3_step(pStmt);
        int i = 0;
        int column_count = sqlite3_column_count(pStmt);
        for (i = 0; i < column_count; i++)
        {
        	char *datatype = NULL;
        	const char *collation;
        	int not_null;
        	int primary_key;
        	int autoincrement;

        	sqlite3_table_column_metadata( db, NULL, tableName, sqlite3_column_name(pStmt, i), (const char **) &datatype, &collation, &not_null, &primary_key, &autoincrement);
        	if (datatype != NULL)
        		addKeyVal ( (char *) sqlite3_column_name(pStmt, i), datatype, list);
        	else
        		addKeyVal ( (char *) sqlite3_column_name(pStmt, i), "", list);

        	//printf ("%d. name=%s, type%s.\n", i, (char *) sqlite3_column_name(pStmt, i),	 datatype);
        }
        result = sqlite3_finalize(pStmt);
    } while ( result == SQLITE_SCHEMA);
	return list->count;
}

int delete_table (sqlite3 *db, const char *table_name)
{
	char *SQL_ = "drop table if exists ";
	char *SQL = NULL;
    int result = 0;

	SQL = (char *) malloc (sizeof (char) * ( strlen (table_name) + strlen (SQL_) + 2));
	sprintf (SQL, "%s%s", SQL_, table_name);

	result = sqlite3_exec(db, SQL, 0, 0, 0);

	return result;
}

int printf_db_struct ( sqlite3 *db )
{
	int i = 0;
	int j = 0;
	List_t tables;
	int result = 0;
	initList( &tables, 10 );
	result = get_tables_names(db, &tables);
	CHECK_DB_ERROR;
	for ( i = 0 ;i < tables.count; i++)
	{
		printf ( "%s \n", tables.list[i] );
		KeyValueList_t tableStruct;
		initKVList( &tableStruct, 10 );
		get_table_struct( db, &tableStruct, tables.list[i]);
		for ( j = 0; j < tableStruct.count; j++)
		{
			printf ("\t %s \n", tableStruct.pKey[j]);
		}
	}
	return 0;
}

int create_Table_regular (sqlite3 *db, List_t *list)
{
	const char *SqlCreatTemplate = "CREATE TABLE %s (ID INTEGER PRIMARY KEY NOT NULL UNIQUE %s)";
	const char *SqlContentLength = ", CONTENT_LENGTH INTEGER ";
	const char *SqlTEXT_FILED = "TEXT ";
	char *SQL = NULL;
	char *buff = NULL;
    int max_buff_len = MAX_BUFF;
    int buff_len = 0;
    int i = 0;
    int result;
	buff = malloc( sizeof (char) * MAX_BUFF);
	if (list != NULL)
	{
		for (i = 0; i < list->count; i++)
		{
			if ((MY_STRNCMP (list->list[i], CONTETN_LENGTH_NAME)) == 0)
			{
				if (checkmemsize( buff_len + strlen(SqlContentLength) + 4, &max_buff_len, buff) == NULL)
					return -1;
				sprintf(buff + buff_len, "%s", SqlContentLength);
				buff_len = strlen(buff);
			}
			else
			{
				if ((checkmemsize( buff_len + strlen(list->list[i]) + strlen(SqlTEXT_FILED) + 4, &max_buff_len, buff)) == NULL)
					return -1;
				sprintf(buff + buff_len, ", %s %s", list->list[i], SqlTEXT_FILED);
				buff_len = strlen(buff);
			}
		}
	}
	else
	{
		sprintf (buff, "%s", SqlContentLength );
	}
	SQL = (char *) malloc( sizeof(char) * ( strlen ( DB_TABLE_NAME ) + strlen(buff) + strlen(SqlCreatTemplate) + 1));
	sprintf(SQL, SqlCreatTemplate, DB_TABLE_NAME, buff);
	free(buff);
	// try to create new table
	result = sqlite3_exec(db, SQL, 0, 0, 0);
	if (result == 1 )
		if ( (result = delete_table (db, DB_TABLE_NAME)) != SQLITE_OK)
			return -1;
		else
			result = sqlite3_exec(db, SQL, 0, 0, 0);
	return result;
}

int create_Table_FTS (sqlite3 *db, List_t *list)
{
	const char *SqlCreatTemplate = "CREATE VIRTUAL TABLE %s USING fts4 (ID TEXT %s )";
//	const char *SqlCreatTemplate = "CREATE TABLE %s (ID INTEGER PRIMARY KEY NOT NULL UNIQUE %s )";
//	const char *SqlContentLength = ", CONTENT_LENGTH INTEGER ";
	const char *SqlTEXT_FILED = "TEXT ";
	char *SQL = NULL;
	char *buff = NULL;
    int max_buff_len = MAX_BUFF;
    int buff_len = 0;
    int i = 0;
    int result;
	buff = malloc(MAX_BUFF);
	for (i = 0; i < list->count; i++)
	{
/*
		if ((MY_STRNCMP (list->list[i], CONTETN_LENGTH_NAME)) == 0)
		{
			if (checkmemsize( buff_len + strlen(SqlContentLength) + 4, &max_buff_len, buff) == NULL)
				return -1;
			sprintf(buff + buff_len, "%s", SqlContentLength);
			buff_len = strlen(buff);
		}
*/
//		else
//		{
		if ((checkmemsize( buff_len + strlen(list->list[i]) + strlen(SqlTEXT_FILED) + 4, &max_buff_len, buff)) == NULL)
			return -1;
		sprintf(buff + buff_len, ", %s %s", list->list[i], SqlTEXT_FILED);
		buff_len = strlen(buff);
//		}
	}
	SQL = (char *) malloc( sizeof(char) * ( strlen ( DB_TABLE_NAME_FTS ) + strlen(buff) + strlen(SqlCreatTemplate) + 1));
	sprintf(SQL, SqlCreatTemplate, DB_TABLE_NAME_FTS, buff);
	free(buff);
	// try to create new table
	printf ("%s \n", SQL);
	result = sqlite3_exec(db, SQL, 0, 0, 0);
	if (result == 1 )
		if ( (result = delete_table (db, DB_TABLE_NAME_FTS)) != SQLITE_OK)
			return -1;
		else
			result = sqlite3_exec(db, SQL, 0, 0, 0);
	return result;
}

void databaseError(sqlite3* db){
        int errcode = sqlite3_errcode(db);
        const char *errmsg = sqlite3_errmsg(db);
        fprintf(stdout, "Database error %d: %s\n", errcode, errmsg);
}

