/*
 * keyvalstr.c
 *
 *  Created on: Feb 6, 2014
 *      Author: Volodymyr Varchuk
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>

#include "strfunc.h"
#include "filefunc.h"

#define KEY_OR_VAL flag == 1 ? kvList->pKey[i] : kvList->pVal[i]
//#define KEY_OR_VAL_SIZE flag == 1 ? 3 : 5
#define KEY_OR_VAL_TEMPL flag == 1 ? " %s," : " \"%s\","



unsigned long num_CRC32 (const char * str)
{
	unsigned long crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, str, strlen(str));
	return crc;
}

char * str_CRC32 (const char * str)
{
	unsigned long crc = crc32(0L, Z_NULL, 0);
	char *	buff = NULL;
	crc = crc32(crc, str, strlen(str));
	buff = (char *) malloc (sizeof (char) * 13);
	sprintf( buff, "%zu", crc );
	return buff;
}

char *getInsert_Values_Str ( KeyValueList_t *kvList, List_t *fields, int flag )
{
	int i = 0, j = 0;
	char *str = NULL;
	int strsize = 0;
	for (i = 0; i < kvList->count; i++)
	{
		for ( j = 0; j < fields->count; j++)
			if (strcasecmp( kvList->pKey[i] , fields->list[j] ) == 0 )
				strsize += strlen ( KEY_OR_VAL ) + strlen (KEY_OR_VAL_TEMPL);
	}
	strsize++;

	str = (char *) malloc ( strsize * sizeof (char ) );
	if (str == NULL )
		return NULL;
	str[0] = '\0';
	for (i = 0; i < kvList->count; i++)
	{
		for ( j = 0; j < fields->count; j++)
			if ( strcasecmp( kvList->pKey[i], fields->list[j] ) == 0 )
			{
				sprintf ( str + strlen (str), KEY_OR_VAL_TEMPL, KEY_OR_VAL );
			}
	}
	str [strlen ( str ) - 1] = '\0';
	return str;
}

/*
char *getInsert_Values_Str ( KeyValueList_t *kvList, List_t *fields, int flag )
{
	int i = 0, j = 0;
	char *str = NULL;
	int strsize = 0;
	for (i = 0; i < kvList->count; i++)
	{
		for ( j = 0; j < fields->count; j++)
			if (strcasecmp( kvList->pKey[i] , fields->list[j] ) == 0 )
				strsize += strlen ( KEY_OR_VAL ) + 3;
	}
	strsize++;
	str = (char *) malloc ( strsize * sizeof (char ) );
	if (str == NULL )
		return NULL;
	str[0] = '\0';
	for (i = 0; i < kvList->count; i++)
	{
		for ( j = 0; j < fields->count; j++)
			if ( strcasecmp( kvList->pKey[i], fields->list[j] ) == 0 )
				sprintf ( str + strlen (str), " %s,", KEY_OR_VAL );
	}
	str [strlen ( str ) - 1] = '\0';
	return str;
}
*/

char *getKeyArgs (char * pKeyVal)
{
	char *pKey = NULL;
	char *p = NULL;
	pKey = (char *) malloc (sizeof (char) * (strlen (pKeyVal) + 1));

	if ((p = strstr ( pKeyVal, ":")) != NULL)
	{
		strncpy (pKey, pKeyVal, p - pKeyVal);
		pKey[p-pKeyVal] = '\0';
	}
	else
		return NULL;
	return pKey;
}

char *getValArgs (char * pKeyVal)
{
	char *pVal = NULL;
	char *p = NULL;
	pVal = (char *) malloc (sizeof (char) * (strlen (pKeyVal) + 1));

	if ((p = strstr ( pKeyVal, ":")) != NULL)
	{
		strncpy (pVal, p + 1, strlen (pKeyVal) + 1);
	}
	else
		return NULL;
	return pVal;
}

char *getKeyArgs_e (char * pKeyVal)
{
	char *pKey = NULL;
	char *p = NULL;

	if ((p = strstr ( pKeyVal, "=")) != NULL)
	{
		pKey = (char *) malloc ( sizeof (char ) * (p - pKeyVal + 2) );
		strncpy (pKey, pKeyVal, p - pKeyVal );
		pKey[p - pKeyVal] = '\0';
	}
	else
		return NULL;

	return pKey;
}

char *getValArgs_e (char * pKeyVal)
{
	char *pVal = NULL;
	char *p = NULL;



	//pVal = (char *) malloc (sizeof (char) * (strlen (pKeyVal) + 2));

	if ((p = strstr ( pKeyVal, "=")) != NULL)
	{
		int slen = strlen (pKeyVal) - (p - pKeyVal);

		pVal = (char *) malloc (sizeof (char) * (slen + 2));
		strncpy (pVal, p + 1, slen + 1);
	}
	else
		return NULL;
	return pVal;
}



char ** checkmemsize2 (int curlen, int *max_len, char** buff)
{
	int max_len_temp = *max_len;

	if (curlen >= (max_len_temp - 1))
	{
		printf ("realloc  %d, %d \n", curlen, max_len_temp);
		char **temp_list = NULL;
		printf ("%p, %zu\n", buff, sizeof (char *) * max_len_temp*2);
		temp_list = (char **) realloc (buff, sizeof (char *) * max_len_temp*2 + 1);
		printf (" realloc OK\n");
		if (temp_list != NULL)
		{
			buff = temp_list;
			max_len_temp *= 2;
		}
		else
		{
			return NULL;
		}
	}
	*max_len = max_len_temp;
	return buff;
}

char *my_strtolower (char *str)
{

	if (str == NULL)
		return NULL;
	int i = 0;
	char *resstr = (char *) malloc (sizeof (char) * strlen (str) + 1);

	for (i = 0; i < strlen (str); i++)
	{
		resstr[i] = tolower (str[i]);
	}
	resstr[i] = '\0';
	return resstr;
}

char *checkmemsize (int curlen, int *max_len, char* buff)
{
	char *temp;
	if (curlen >= *max_len)
	{
		temp = (char *) realloc (buff, sizeof (char) * ((int) (*max_len) + 1) * 2 );
		if (temp == NULL)
			return NULL;
		buff = temp;
		*max_len = *max_len * 2;
		return  (char *) buff;
	}
}

/*
//////////////////////////////////////////////////////////////////////////////////////////////
work with KeyValueList_t struct
1. add char* key  and char* value
	adding key and value to list
	increment count of key-value items
	increase max_count if the limit is reached

2. init key and value list
//////////////////////////////////////////////////////////////////////////////////////////////
*/

int addKeyVal (char *key, char *val, KeyValueList_t *tKVList)
{
	int resize = 0;
	int max_count = tKVList->max_count;
	char * buff_key = NULL;
	char * buff_val = NULL;
	char **temp_key = NULL;
	char **temp_val = NULL;

	if (key != NULL)
		buff_key = (char *) malloc (sizeof (char) * (strlen (key) + 1));
	else
		return -1;

	if (val != NULL )
		buff_val = (char *) malloc (sizeof (char) * (strlen (val) + 1));
	else
		return -1;

	if ( tKVList->count >= tKVList->max_count )
	{
		temp_key = (char **) realloc (tKVList->pKey, sizeof (char *) * (max_count * 2 + 1));
		temp_val = (char **) realloc (tKVList->pVal, sizeof (char *) * (max_count * 2 + 1));
		if (temp_key != NULL && temp_val != NULL)
		{
			resize = 1;
		}
		else
			return -1;
	}


	if (buff_key != NULL && buff_val != NULL)
	{
		if (resize == 1)
		{
			tKVList->pKey = temp_key;
			tKVList->pVal = temp_val;
			max_count *= 2;
		}

		// add key
		if (key != NULL)
		{
			tKVList->pKey[tKVList->count] = (char *) malloc (sizeof (char) * (strlen (key) + 1));
			strncpy(tKVList->pKey[tKVList->count], key, strlen (key) + 1);
		}
		else
			tKVList->pKey[tKVList->count] = NULL;

		//add val
		if (val != NULL)
		{
			tKVList->pVal[tKVList->count] = (char *) malloc (sizeof (char) * (strlen (val) + 1));
			strncpy(tKVList->pVal[tKVList->count], val, strlen (val) + 1 );
		}
		else
			tKVList->pVal[tKVList->count] = NULL;

		tKVList->count++;
		tKVList->max_count = max_count;
	}

	return 0;
}

int initKVList (KeyValueList_t *tKVList, int size)
{
	if (size <= 0)
		return -1;

	tKVList->max_count = size;
	char **temp_k = NULL;
	char **temp_v = NULL;

	temp_k = (char **) malloc (sizeof (char * ) * (size + 1));
	temp_v = (char **) malloc (sizeof (char * ) * (size + 1));
	tKVList->count = 0;
/*
	if ( (tKVList->pKey == NULL) || ( tKVList->pVal == NULL ))
		return -1;
*/
	tKVList->pKey = temp_k;
	tKVList->pVal = temp_v;
	return size;
}



/*
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////

*/

int addToList (const char *str, List_t *tList)
{
	int resize = 0;
	int max_count = tList->max_count;
	char *temp_buff = NULL;

	//temp_buff = (char *) malloc( sizeof (char) * strlen (str) );
	if ( tList->count >= tList->max_count )
	{
		char **temp_buff  = NULL;
		temp_buff = ( char ** ) realloc ( tList->list, sizeof (char *) * ( tList->max_count * 2 ));
		if (temp_buff != NULL)
		{
			tList->list = temp_buff;
			tList->max_count = tList->max_count * 2;
		}
		else
			return -1;
	}

	//
	if (str != NULL)
	{
		tList->list[ tList->count ] = (char *) malloc (sizeof (char) * (strlen (str) + 1));
		strncpy(tList->list[tList->count], str, strlen (str) + 1);
	}
	else
	{
		tList->list[tList->count] = NULL;
	}
	tList->count++;

	return 0;
}

int initList (List_t * list, int size)
{
	if (size <= 0)
		return -1;

	list->max_count = size;
	list->list = (char **) malloc (sizeof (char * ) * size);
	list->count = 0;
	if ( list->list == NULL)
		return -1;

	return size;
}



/*

//////////////////////////////////////////////////////////////////////////////////////////////
work with IntKeyValueList_t struct

1. add char* key  and int value
	adding key and value to list
	increment count of key-value items
	increase max_count if the limit is reached

2. init key and value list
//////////////////////////////////////////////////////////////////////////////////////////////

*/

int addIntKeyVal (char *key, int val, IntKeyValueList_t *tKVList)
{
	int resize = 0;
	int max_count = tKVList->max_count;
	char * buff_key = NULL;
	int buff_val = 0;
	char **temp_key = NULL;
	int *temp_val = NULL;

	if (key != NULL)
		buff_key = (char *) malloc (sizeof (char) * (strlen (key) + 1));

	if ( tKVList->count >= tKVList->max_count )
	{
		temp_key = (char **) realloc (tKVList->pKey, sizeof (char *) * (max_count * 2));
		temp_val = (int *) realloc (tKVList->pVal, sizeof (int ) * (max_count * 2));
		if (temp_key != NULL && temp_val != NULL)
		{
			resize = 1;
		}
		else
			return -1;
	}
	if (buff_key != NULL )
	{
		if (resize == 1)
		{
			tKVList->pKey = temp_key;
			tKVList->pVal = temp_val;
			max_count *= 2;
		}

		// add key
		if (key != NULL)
		{
			tKVList->pKey[tKVList->count] = (char *) malloc (sizeof (char) * (strlen (key) + 1));
			strncpy(tKVList->pKey[tKVList->count], key, strlen (key) + 1);
		}
		else
			tKVList->pKey[tKVList->count] = NULL;

		//add val
		tKVList->pVal[tKVList->count] = val;

		tKVList->count++;
		tKVList->max_count = max_count;
	}

	return 0;
}

int initIntKVList (IntKeyValueList_t *tKVList, int size)
{
	if (size <= 0)
		return -1;

	tKVList->max_count = size;
	tKVList->pKey = (char **) malloc (sizeof (char * ) * size);
	tKVList->pVal = (int *) malloc (sizeof (int) * size);
	tKVList->count = 0;
	if ( (tKVList->pKey == NULL) || ( tKVList->pVal == NULL ))
		return -1;

	return size;
}

void printIntKVList (IntKeyValueList_t *list)
{
	int i = 0;
	for (i = 0; i < list->count; i++)
	{
		printf ("%d. key=%s, val=%d\n", i, list->pKey[i], list->pVal[i]);
	}
}
