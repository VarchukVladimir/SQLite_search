/*
 * filefunc.c
 *
 *  Created on: Feb 12, 2014
 *      Author: Volodymyr Varchuk
 */


#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "dbfunc.h"
#include "filefunc.h"
#include "strfunc.h"

char *gen_db_file_name (const char * db_dir, const char *db_file)
{

	char *db_file_full_path = NULL;

	db_file_full_path = (char *) malloc ( sizeof (char) * ( strlen (db_dir) + strlen (db_file) + 2));

	sprintf (db_file_full_path, "%s/%s", db_dir, db_file);

	return db_file_full_path;
}

int newbufferedunpack (char * dir_name, char * devname)
{

//	printf ("%s %s \n", dir_name, devname);
	int fdinfile;
	char *dirName = dir_name;
  	DIR *dir;
	dir = opendir(dirName);
	if (dir == NULL)
	{
		int result=mymakedir(dirName);
		if (result != 0)
		{
			printf ("Error. Can`t create directory %s\n", dirName);
			return -1;
		}
	}
	else
		closedir (dir);
	int MAXREAD = 1024;
	char readbuf [MAXREAD];

	int letcount;
	letcount = 0;
	fdinfile = open (devname, O_RDONLY);
	if (fdinfile <= 0)
	{
		return -1;
	}
	int bread = 1;
	long deltabytes = 0;
	long mainbytes = 0;
	int filecount = 0;


	char blocksizestr [10];
	size_t blocksize;
	while (bread > 0)
	{
		if (filecount > 50)
		{
			printf ("Too many files in archive\n");
			break;
		}
		// получение количества байт в имени сохраненного файла
		bread = read(fdinfile, blocksizestr, 10);
		if (bread <= 0)
		{
			break;
		}
		blocksizestr[10] = '\0';
		blocksize = atoi (blocksizestr);

		// read file name
		bread = read (fdinfile, readbuf, blocksize);
		readbuf [blocksize] = '\0';

		// получение количества байт в сохраненном файле
		bread = read(fdinfile, blocksizestr, 10);
		blocksizestr[10] = '\0';
		blocksize = atoi (blocksizestr);

		size_t filelen = 0;
		size_t total_bytes = 0;
		filelen = blocksize;

		int fdefile;
		fdefile = open (readbuf, O_WRONLY | O_CREAT | O_TRUNC, S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR);
		if (fdefile <= 0)
		{
			printf ("*** ZVM Error open %s file for write. Increasing the current offset of the file descriptor on %zu bytes  and skip save this file.\n", readbuf, filelen);
			// add check type of device. If we can use lseek
			bread = lseek (fdinfile, filelen, SEEK_CUR);
			continue;
		}

		// readind and save data file
		char *buff = NULL;
		size_t blockreadsize = READ_WRITE_BUFF_SIZE;

		int readb = 0;//read(fdinfile, buff,filelen);
		int writeb = 0;//write(fdefile, buff,readb);

		long countreadbytes = 0;
		long countwritebytes = 0;

		buff = (char *) malloc (blockreadsize * sizeof (char));
		if (filelen > 0)
		{

			if (blockreadsize > filelen)
				blockreadsize = filelen;
			int i=filelen / blockreadsize + 1;
			for (; i > 0; i--)
			{
				if (filelen < blockreadsize)
					blockreadsize = filelen;
				if (i == 1)
					blockreadsize = filelen - countreadbytes;
				readb = read(fdinfile, buff, blockreadsize);
				if (readb > 0)
					writeb = write(fdefile, buff, readb);
				else
					writeb = readb;
				countreadbytes += readb;
				countwritebytes += writeb;
			}
			readb = countreadbytes;
			writeb = countwritebytes;
		}


		// statistic
		total_bytes += filelen;
		char buff_filelen[15];
		sprintf (buff_filelen, "%zu", filelen);
		filecount++;


		free (buff);
		close (fdefile);

		if (readb != writeb)
		{
			return -1;
		}
	}
	close (fdinfile);
}


int newbufferedpack (char *devname, char *dirname )
{
	int fdpackfile;
	fdpackfile = open (devname, O_WRONLY | O_CREAT | O_TRUNC, S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR);
	if ( fdpackfile  <= 0 )
	{
		printf ("*** ZVM Error open pack file (write)%s\n", devname);
		return -1;
	}

	char *indexpath = NULL;
	indexpath = (char *) malloc (sizeof (char ) * (strlen (dirname) + 1));
	sprintf(indexpath, "%s", dirname);
  	DIR *dir;
	struct dirent *entry;

	indexpath [strlen (indexpath)] = '\0';

	dir = opendir(indexpath);

	char *newpath = NULL;

//	printf ("OK - - 1\n");

	if (!dir)
		printf ("*** ZVM Error open DIR %s\n", indexpath);
	int blocksize = READ_WRITE_BUFF_SIZE;
	char *buff = NULL;
	buff = (char *) malloc (sizeof (char) * blocksize);

	long deltabytes = 0;
	long mainbytes = 0;
	int filecount = 0;


	while((entry = readdir(dir)))
	{
		if(entry->d_type != DT_DIR)
		{
			size_t size;
			size_t bread = 0;
			size_t bwrite;
			size_t bytecount;
			bytecount = 0;
			newpath = (char *) malloc (strlen (entry->d_name) + strlen(indexpath) + 2);
			sprintf(newpath, "%s/%s", indexpath, entry->d_name);
			int fd;

			fd = open (newpath, O_RDONLY);
			size = getfilesize_fd(fd, NULL, 0);

			char tempstr [strlen (newpath) + 12];
			// write header (10 bytes size of filename + filename + 10 bytes size of filedata)
			sprintf(tempstr, "%10zu%s%10zu", strlen (newpath), newpath, size);
			bwrite = write (fdpackfile, tempstr, strlen (tempstr));
			// write header (10 bytes size of filename + filename)


			//read and write file data
			if (size > 0)
			{
				while ((bread = read(fd, buff, blocksize)) > 0)
				{
					bytecount += bread;
					bwrite = write(fdpackfile, buff, bread);
				}
			} else
				bytecount = 0;

			close (fd);

			// for statistic data
			char buff_size[15];
			sprintf (buff_size, "%zu", size);
			//	addKeyVal(newpath, buff_size, KVist);
/*
			char *indexnameptr = NULL;
			if ((indexnameptr = strstr (newpath,DELTAINDEX)) != NULL )
				deltabytes += size;

			indexnameptr = NULL;
			if ((indexnameptr = strstr (newpath,MAININDEX)) != NULL )
				mainbytes += size;
			filecount++;
*/
			//statistic
		}
	}
	free (buff);
	close (fdpackfile);

}


int mymakedir (char * newdir)
{
  char *buffer ;
  char *p;
  int  len = (int)strlen(newdir);

  if (len <= 0)
    return 0;

  buffer = (char*)malloc(len+1);
        if (buffer==NULL)
        {
                printf("Error allocating memory\n");
                return 1;
        }
  strcpy(buffer,newdir);

  if (buffer[len-1] == '/') {
    buffer[len-1] = '\0';
  }
  if (mkdir (buffer,0777) == 0)
    {
      free(buffer);
      return 0;
    }

  p = buffer+1;
  while (1)
    {
      char hold;

      while(*p && *p != '\\' && *p != '/')
        p++;
      hold = *p;
      *p = 0;
      if ((mkdir (buffer,0777) == -1) && (errno == ENOENT))
        {
          printf("couldn't create directory %s\n",buffer);
          free(buffer);
          return 0;
        }
      if (hold == 0)
        break;
      *p++ = hold;
    }
  free(buffer);
  return 0;
}

size_t getfilesize_fd (int fd, char *filename, int sizebyfilename)
{
	if (sizebyfilename == 1)
		fd = open (filename, O_RDONLY);
	if (fd < 0)
	{
		printf ("*** ZVM Error get filesize wrong file descriptor %d\n", fd);
		return -1;
	}

	long fsize;
	fsize = lseek (fd, 0, SEEK_END);
	if (fsize < 0)
	{
		printf ("*** ZVM Error get filesize for file descriptor %d\n", fd);
		if (sizebyfilename == 1)
			close(fd);
		return -1;
	}
	int temp = lseek (fd, 0, SEEK_SET);
	if (temp < 0)
		printf ("*** ZVM Warning! I can't SEEK_SET execute for file descriptor %d\n", fd);

	if (sizebyfilename == 1)
		close(fd);

	return fsize;
}

void mylistdir (char *path)
{
  	DIR *dir;
	struct dirent *entry;
	char newpath[1024];
	dir = opendir(path);
	int len;
	if(dir == 0)
	{
		return;
	}
	while((entry = readdir(dir)))
	{

		printf ("%s/%s D_TYPE = %d\n",path, entry->d_name, entry->d_type);
		if(entry->d_type == DT_DIR)
		{
			if (strcmp (entry->d_name, ".") != 0 && strcmp (entry->d_name, "..") != 0)
			{
				strcpy (newpath, path);
				len = strlen (newpath);
				if (newpath [len-1] != '/')
					strcat (newpath, "/");
				strcat (newpath, entry->d_name);
				mylistdir (newpath);
			}
		}
	}
	closedir(dir);
}


