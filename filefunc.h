/*
 * filefunc.h
 *
 *  Created on: Feb 12, 2014
 *      Author: volodymyr varchuk
 */

#ifndef FILEFUNC_H_
#define FILEFUNC_H_

#define READ_WRITE_BUFF_SIZE 1024 * 64



size_t getfilesize_fd (int, char *, int);
char *gen_db_file_name (const char *, const char *);
int newbufferedunpack (char *, char *);
int newbufferedpack (char *, char * );

#endif /* FILEFUNC_H_ */
