/*
 *   Associate user defined data to file descriptors. (thread-safe).
 *
 *   Copyright (C) 2019  Renzo Davoli <renzo@cs.unibo.it> VirtualSquare team.
 *
 *   This library is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2.1 of the License, or (at
 *   your option) any later version.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this library; if not, write to the Free Software Foundation,
 *   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *   Example of usage:
 *      //fduserdata uses a trivial hash table, the optional arg is the
 *      // size of the hash table: default value = 64
 *      FDUSERDATA table = fduserdata_create(0);
 *      
 *			struct mydata {
 *        // fd data fields ...
 *      };
 *      // create a struct mydata for the file descriptor fd.
 *      struct mydata *data = fduserdata_new(table, fd, struct mydata);
 *      //.... set user defined data (data->fields)
 *      fduserdata_put(data);
 *
 *      // search for data 
 *      // there is mutual exclusion between new/put, get/put (or new/del, get/del)
 *      // so do not insert time consuming or blocking ops.
 *      struct mydata *fddata = fduserdata_get(table, fd);
 *      if (fddata) {
 *                     //... read/update user defined data (data->fields)
 *                     fduserdata_put(data);
 *                     // use fduserdata_del instead of fduserdata_put to
 *                     // delete the element
 *      }
 *
 *      // at the end... when table is no longer required
 *      fduserdata_destroy(table);
 *     
 */

#ifndef FDUSERDATA_H
#define FDUSERDATA_H

struct fduserdata_table;
typedef struct fduserdata_table FDUSERDATA;

FDUSERDATA *fduserdata_create(int size);
void fduserdata_destroy(FDUSERDATA *fdtable);

#define fduserdata_new(fdtable, fd, type) ((type *)(__fduserdata_new((fdtable),(fd),sizeof(type))))
void *__fduserdata_new(FDUSERDATA *fdtable, int fd, size_t count);

void *fduserdata_get(FDUSERDATA *fdtable, int fd); 

void fduserdata_put(void *data);

int fduserdata_del(void *data);

#endif
