<!--
.\" Copyright (C) 2019 VirtualSquare. Project Leader: Renzo Davoli
.\"
.\" This is free documentation; you can redistribute it and/or
.\" modify it under the terms of the GNU General Public License,
.\" as published by the Free Software Foundation, either version 2
.\" of the License, or (at your option) any later version.
.\"
.\" The GNU General Public License's references to "object code"
.\" and "executables" are to be interpreted as the output of any
.\" document formatting or typesetting system, including
.\" intermediate and printed output.
.\"
.\" This manual is distributed in the hope that it will be useful,
.\" but WITHOUT ANY WARRANTY; without even the implied warranty of
.\" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
.\" GNU General Public License for more details.
.\"
.\" You should have received a copy of the GNU General Public
.\" License along with this manual; if not, write to the Free
.\" Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
.\" MA 02110-1301 USA.
.\"
-->

# NAME

fduserdata\_create, fduserdata\_destroy, fduserdata\_destroy\_cb, fduserdata\_new, fduserdata\_get, fduserdata\_put,
fduserdata\_del	- associate file descriptors with user defined data

# SYNOPSIS

`#include *fduserdata.h*`

`FDUSERDATA *fduserdata_create(int ` _size_`);`

`void fduserdata_destroy(FDUSERDATA *`_fdtable_`);`

`typedef void (*fduserdata_destr_cb_t)(int fd, void *data, void *arg);` *br/*
`void fduserdata_destroy_cb(FDUSERDATA *`_fdtable_`, fduserdata_destr_cb_t ` _callback_`, void *`_arg_`);`

`void *fduserdata_new(FDUSERDATA *`_fdtable_`, int ` _fd_`, ` _type_`);`

`void *fduserdata_get(FDUSERDATA *`_fdtable_`, int ` _fd_`); `

`void fduserdata_put(void *`_data_`);`

`int fduserdata_del(void *`_data_`);`

# DESCRIPTION

This library permits one to associate file descriptors with user defined data, more precisely it manages
a data structure whose searching key is a file descriptor.

`fduserdata_create` and `fduserdata_destroy` are the constructor and destructor of the data structure, respectively.
The data structure has been implemented as a hash table, the argument _size_ of `fduserdata_create` is the size of
the hash array. When _size_ is zero the hash array has its default size (64).

`fduserdata_destroy_cb` is an alternative destructor which calls the function _callback_ for
each element still in the data structure.

`fduserdata_new` creates a new element. It is a macro: _type_ is the type of the user data.

`fduserdata_get` search the user data associated to the _fd_.

Both `fduserdata_new` and `fduserdata_get` lock the access to the element, so that fduserdata is thread safe.
`fduserdata_put` unlocks the element and makes it available for further requests.

`fduserdata_del` can be used instead of `fduserdata_put` to delete the element.

# RETURN VALUE
`fduserdata_create` returns the descriptor of the data structure (NULL in case of error).

`fduserdata_new` returns the element of type _type_ just created (NULL in case of error).

`fduserdata_get` returns the element or NULL if no data corresponds to the file descriptor _fd_.

`fduserdata_del` On success, zero is returned.  On error, -1 is returned.

On error, _errno_ is set appropriately.

# EXAMPLE

fduserdata uses a trivial hash table, the optional arg is the size of the hash table: default value = 64
```
  FDUSERDATA table = fduserdata_create(0);

  struct mydata {
  // fd data fields ...
     };
```

create a struct mydata for the file descriptor fd.

```
  struct mydata *data = fduserdata_new(table, fd, struct mydata);
```

.... set user defined data (data-*fields)

```
  fduserdata_put(data);
```

search for data
there is mutual exclusion between new/put, get/put (or new/del, get/del) so do not insert time consuming or blocking ops.

```
  struct mydata *fddata = fduserdata_get(table, fd);
  if (fddata) {
```

... read/update user defined data (data-\>fields)
(use fduserdata\_del instead of fduserdata\_put to delete the element)
```
      fduserdata_put(data);
  }
```

at the end... when table is no longer required

```
  fduserdata_destroy(table);
```

# AUTHOR
VirtualSquare. Project leader: Renzo Davoli.
