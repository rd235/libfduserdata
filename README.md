# libfduserdata

associate file descriptors with user defined data

This small library permits to associate user defined data to file descriptors. This library is thread safe.

The library uses a hash table.

## Initialization/Termination

The function `fduserdata_create` sets up the data structure.
e.g.

    FDUSERDATA table = fduserdata_create(0);

The optional parameter (here 0) is the size of the hashtable. The default size if 64.

When the data structure is no longer needed it can be deleted/freed using:

    fduserdata_destroy(table);

## create a new element:
Allocate a `struct mydata` element connected to the file descriptor `fd` in `table`:

    struct mydata *data = fduserdata_new(table, fd, struct mydata);
    // mydata.xxxx = yyyy
    // intialize all mydata  fields
    fduserdata_put(data)

Please note that `fduserdata_put` is required to close the critical section. No other threads can add, read or modify elements in `table` until the current element has been completely initialized and `fduserdata_put` has been called.

## search an element/modify

    struct mydata *fddata = fduserdata_get(table, fd);
    if (fddata) {
        //... read/update user defined data (data->fields)
        fduserdata_put(data);
    }

Again `fduserdata_put` closes the critical section. Slow or blocking operations should not take place between `fduserdata_get` and `fduserdata_put`

## delete an element
    struct mydata *fddata = fduserdata_get(table, fd);
    if (fddata)
        fduserdata_del(data);

`fduserdata_get` (or `fduserdata_new`) is needed before `fduserdata_del` because `fduserdata_del` deletes the element while it closes the critical section.
(in this way it is not possible to delete an element twice).

# compile and install the library:

This is a sequence of commands that can be used to compile install the library.

    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    