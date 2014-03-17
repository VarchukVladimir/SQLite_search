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
	int bytesave = 0;
	int bread = 0;
	char buff [BUFF_SIZE];
	int file_save_fd = 0;
	int listlen = 0;
	int listlen_max = INITIAL_LIST_LEN;
	char ** templist = NULL;

//	templist = (char ** ) malloc (sizeof (char *) * INITIAL_LIST_LEN);

/*
	if (file_save != NULL)
		file_save_fd = open (file_save, O_WRONLY | O_CREAT | O_TRUNC, S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR);
*/

	int i = 0;
	while (fgets (buff, BUFF_SIZE, f) != NULL )
	{
		if (strlen (buff) > 1 )
		{
			//char *temp = NULL;
			printf (" buff = %s. \n", buff);
			list->list[i] = (char * ) malloc (sizeof (char) * (strlen (buff) + 1));
			buff [strlen (buff) - 1] = '\0';
			strncpy (list->list[i], buff, strlen (buff));
			printf (" buff1 = %s. \n", list->list[i]);
			printf ("\n\n", list->list[i]);


			//temp_list = (char **) checkmemsize2 (listlen, &listlen_max, templist );
/*
			if (list->count >= list->max_count)
			{
				char **temp_list = NULL;
				temp_list = (char ** ) realloc( list->list, sizeof (char *) * ( list->max_count * 2 ) );
				if (temp_list != NULL )
				{
					list->list = temp_list;
					list->max_count *= 2;
				}
				else
					return -1;
			}
*/
			//list->list[i] = temp;
			//printf (" - %s \n", list->list[i]);
			list->count++;
			i++;
		}
	}

	for ( i  = 0; i < list->count; i++)
		printf ( ",,,, %s \n", list->list[i] );

/*
	if (file_save_fd > 0)
	{
		int i = 0;
		for (i = 0; i < listlen; i++)
			write (file_save_fd, list->list[i], strlen (templist[i]) + 1);
		close (file_save_fd);
	}
*/

//	list->list = templist;
//	list->count = listlen;

	return listlen;
}

int get_key_val_list ( FILE *f, char * file_save, KeyValueList_t * list )
{
	int bytesave = 0;
	int bread = 0;
	char buff [BUFF_SIZE];
	int file_save_fd = 0;
	int listlen = 0;
	int listlen_max = INITIAL_LIST_LEN;
/*
	char **pKey = NULL;
	char **pVal = NULL;

	pKey = (char ** ) malloc (sizeof (char *) * INITIAL_LIST_LEN);
	pVal = (char ** ) malloc (sizeof (char *) * INITIAL_LIST_LEN);
*/

	if (file_save != NULL)
		file_save_fd = open (file_save, O_WRONLY | O_CREAT | O_TRUNC, S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR);

	int i = 0;
	while (fgets (buff, BUFF_SIZE, f) != NULL )
	{
		if (strlen (buff) > 0 )
		{
			addKeyVal(getKeyArgs(buff), getValArgs(buff), list);
/*
			char *temp = NULL;
			temp = (char * ) malloc (sizeof (char) * (strlen (buff) + 1));
			buff [strlen (buff) - 1] = '\0';
			strncpy (temp, buff, strlen (buff) + 1);
			char **temp_list_key = NULL;
			char **temp_list_val = NULL;

			temp_list_key = (char **) checkmemsize2 (listlen, &listlen_max, pKey);
			temp_list_val = (char **) checkmemsize2 (listlen, &listlen_max, pKey);
			if ((temp_list_key == NULL) || (temp_list_val == NULL))
			{
				list->count = 0;
				list->pKey = NULL;
				list->pVal = NULL;
				free (pKey);
				free (pVal);
				return -1;
			}
			pKey [listlen] = getKeyArgs(buff);
			pVal [listlen] = getValArgs(buff);
			listlen++;
*/
		}
	}

	if (file_save_fd > 0)
	{
		;
	}

/*
	list->pKey = pKey;
	list->pVal = pVal;
	list->count = listlen;
*/

	return list->count;
}


