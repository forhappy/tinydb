tinydb
======

Tinydb is a tiny nosql database supporting pluggable storage engine. It aims to provide a simple but workable nosql database server,
Tinydb's core implemention (Common Storage Abstract Service: csas.h and csas.c) only cost about 200 sloc, it now have a RESTful server
based on libevent2, a persistent storage engine based on google's leveldb and a in-memory storage engine based on uthash(http://uthash.sourceforge.net/).
If you want to take a deep look at its implemention detail, here is the URL(in Chinese)(http://www.cnblogs.com/haippy/archive/2012/11/24/2786404.html)

install
=======
First of all you have to install libevent2 and leveldb ,then compile libevhtp:

> $:cd libevhtp/build

> $:cmake ../.

> $:make

if libevhtp successfull compiled, you will get a static library in libevhtp/build name "libevhtp.a", move it to tinydb/lib/.

Next all you have to do is comiple tinydb, this is easy and you only need to "make" in the directory, since tinydb supports pluggable storage engine,
if you want to implement your storage engine, you will need to fill three function in your engine: put, get, delete, the example engine shows you how 
to do this job.But you may have to take a look at engine-leveldb.h/engine-leveldb.c if you want to make you engine a little more flexible.

storage engine
==============
tinydb supports two simple storage engine: leveldb-based persistent storage engine and uthash(http://uthash.sourceforge.net/) based in-memory 
engine, sqlite based engine will be implemented very soon, and what's more you have the chance to implement your own storage engine,
it's awsome right?, just to keep it simple and beautiful.

How to make it run ?
====================
After you compiled tinydb there is an executable file name "server", just run it in command line with no arguments:
> $: ./server

Open your browser and type something in the URL box, magic will happen here.

Commands
========
Tinydb supports only 3 commands: set, get, delete, to keep its simplicity.

SET
> http:127.0.0.1:8088/set?key=hello&value=world

> {"err": "OK","msg":"Set key successfully."}

GET
> http:127.0.0.1:8088/get?key=hello

> {"key": "hello","val":"world"}

DELETE
> http:127.0.0.1:8088/delete?key=hello

> {"err": "OK","msg":"Delete key successfully."}

Bugs
=====
Tinydb is tiny project worked in my free time but bugs are inevitable, if you have found some bugs please contact haipingf AT gmail DOT com.
I'm happy to hear you voice. :-)
