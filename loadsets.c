/*
 * loadsets.c
 *
 *  Created on: Feb 6, 2014
 *      Author: Volodymyr Varchuk
 */


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "loadsets.h"
#include "strfunc.h"

#define BUFF_SIZE 1024
#define INITIAL_LIST_LEN 10

int get_list ( FILE *f, char * file_save, List_t * list )
{
	char buff [BUFF_SIZE];
	int i = 0;
	while (fgets (buff, BUFF_SIZE, f) != NULL )
	{

		if (strlen (buff) > 1 )
		{
			buff [strlen (buff) - 1] = '\0';
			list->list[i] = (char * ) malloc (sizeof (char) * (strlen (buff) + 1));
			strncpy (list->list[i], buff, strlen (buff) + 1);
			list->count++;
			i++;
		}
	}
	return 0;
}

int get_key_val_list ( FILE *f, char * file_save, KeyValueList_t * list )
{
	int bytesave = 0;
	int bread = 0;
	char buff [BUFF_SIZE];
	int file_save_fd = 0;
	int listlen = 0;
	int listlen_max = INITIAL_LIST_LEN;
	if (file_save != NULL)
		file_save_fd = open (file_save, O_WRONLY | O_CREAT | O_TRUNC, S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR);

	int i = 0;
	while (fgets (buff, BUFF_SIZE, f) != NULL )
	{
		if (strlen (buff) > 0 )
		{
			addKeyVal(getKeyArgs(buff), getValArgs(buff), list);
		}
	}

	if (file_save_fd > 0)
	{
		;
	}
	return list->count;
}


