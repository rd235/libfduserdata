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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <pthread.h>

#define DEFAULT_MASK 0x3f // 63

#ifndef container_of
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) -  offsetof(type,member)))
#endif

struct fduserdata_table;
typedef struct fduserdata_table FDUSERDATA;

struct fduserdata_item {
	struct fduserdata_item *next;
	struct fduserdata_table *fdtable;
	int fd;
	char data[];
};

struct fduserdata_table {
	int mask;
	pthread_mutex_t mutex;
	struct fduserdata_item *table[];
};

static int size2mask(int x) {
	int scan;
	x = x - 1;
	if (x < 0)
		return DEFAULT_MASK;
	else {
		for (scan = -1; scan & x; scan <<= 1)
			;
		return ~scan;
	}
}

FDUSERDATA *fduserdata_create(int size) {
	int mask = size2mask(size);
	struct fduserdata_table *table = calloc(1, sizeof(struct fduserdata_table) + (mask + 1) * sizeof(struct fduserdata_item *));
	if (table != NULL) {
		table->mask = mask;
		pthread_mutex_init(&table->mutex, NULL);
	}
	return table;
}

void fduserdata_destroy(FDUSERDATA *fdtable) {
	if (fdtable != NULL) {
		int i;
		pthread_mutex_lock(&fdtable->mutex);
		for (i = 0; i < (fdtable->mask + 1) ; i++) {
			struct fduserdata_item *fdud = fdtable->table[i];
			while (fdud != NULL) {
				struct fduserdata_item *this = fdud;
				fdud = this->next;
				free(fdud);
			}
		}
		pthread_mutex_unlock(&fdtable->mutex);
		pthread_mutex_destroy(&fdtable->mutex);
		free(fdtable);
	}
}

void *__fduserdata_new(FDUSERDATA *fdtable, int fd, size_t count) {
	if (fdtable != NULL) {
		struct fduserdata_item *fdud = malloc(sizeof(struct fduserdata_item) + count);
		int index;
		if (fdud == NULL)
			return errno = ENOMEM, NULL;
		pthread_mutex_lock(&fdtable->mutex);
		index = fd & fdtable->mask;
		fdud->fd = fd;
		fdud->fdtable = fdtable;
		fdud->next = fdtable->table[index];
		fdtable->table[index] = fdud;
		return fdud->data;
	} else
		return errno = EINVAL, NULL;
}

void *fduserdata_get(FDUSERDATA *fdtable, int fd) {
	if (fdtable != NULL) {
		pthread_mutex_lock(&fdtable->mutex);
		int index = fd & fdtable->mask;
		struct fduserdata_item *fdud;
		for (fdud = fdtable->table[index]; fdud != NULL && fdud->fd != fd; fdud = fdud->next)
			;
		if (fdud == NULL) {
			pthread_mutex_unlock(&fdtable->mutex);
			return errno = EBADF, NULL;
		} else
			return fdud->data;
	} else
		return errno = EINVAL, NULL;
}


static inline __attribute__((always_inline)) struct fduserdata_item *data2item(void *data) {
	return container_of(data, struct fduserdata_item, data);
}

void fduserdata_put(void *data) {
	if (data != NULL) {
		struct fduserdata_item *fdud = data2item(data);
		pthread_mutex_unlock(&fdud->fdtable->mutex);
	}
}


int fduserdata_del(void *data) {
	if (data == NULL)
		return errno = EINVAL, -1;
	else {
		struct fduserdata_item *fdud = data2item(data);
		FDUSERDATA *fdtable = fdud->fdtable;
		int fd = fdud->fd;
		int index = fd & fdtable->mask;
		struct fduserdata_item **sfdud;
		for (sfdud = &(fdtable->table[index]); *sfdud != NULL && (*sfdud) != fdud; sfdud = &((*sfdud)->next))
			;
		if (*sfdud != NULL) {
			*sfdud = fdud->next;
			free(fdud);
			pthread_mutex_unlock(&fdtable->mutex);
			return 0;
		} else {
			pthread_mutex_unlock(&fdtable->mutex);
			return errno = EBADF, -1;
		} 
	}
}

#if 0
int main(int argc, char *argv[]) {
	FDUSERDATA *fdtable = fduserdata_create(0);
	printf("XXX %d\n", sizeof(struct fduserdata_item));
	int *data;
	data = __fduserdata_new(fdtable, 1, sizeof(* data));
	if (data) *data = 1;
	if (data) fduserdata_put(data);
	data = __fduserdata_new(fdtable, 2, sizeof(* data));
	if (data) if (data) *data = 2;
	fduserdata_put(data);
	data = __fduserdata_new(fdtable, 65, sizeof(* data));
	if (data) if (data) *data = 65;
	fduserdata_put(data);
	data = fduserdata_get(fdtable, 1);
	if (data) printf("%d\n",*data);
	if (data) fduserdata_put(data);
	data = fduserdata_get(fdtable, 2);
	if (data) printf("%d\n",*data);
	if (data) fduserdata_put(data);
	data = fduserdata_get(fdtable, 65);
	if (data) printf("%d\n",*data);
	if (data) fduserdata_put(data);
	data = fduserdata_get(fdtable, 2);
	if (data) fduserdata_del(data);
	data = fduserdata_get(fdtable, 3);
	if (data) fduserdata_del(data);
	data = fduserdata_get(fdtable, 2);
	if (data) printf("%d\n",*data); else printf("NULL\n");
	if (data) fduserdata_put(data);
	data = fduserdata_get(fdtable, 1);
	if (data) printf("%d\n",*data); else printf("NULL\n");
	if (data) fduserdata_put(data);
	data = fduserdata_get(fdtable, 1);
	if (data) fduserdata_del(data);
	data = fduserdata_get(fdtable, 1);
	if (data) printf("%d\n",*data); else printf("NULL\n");
	if (data) fduserdata_put(data);
	data = fduserdata_get(fdtable, 65);
	if (data) printf("%d\n",*data); else printf("NULL\n");
	if (data) fduserdata_put(data);
	fduserdata_destroy(fdtable);
}
#endif
