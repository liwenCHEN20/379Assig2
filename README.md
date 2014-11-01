COMPUT 379 ASSIG 2
==================

This is the cmput 379 assignment 2. An simple webserver implementation.

Server_T:
---------

Using posix threads to service requests.

Need to do:
*	Free malloced resources in main thread. (free them in thread_Starter)
*	Change attributes to start in kernel detached mode.

Server_F:
---------

Using fork() to service requests.

Essentially done


BOTH:
-----

*	need to handle client disconnects and log them.
*	need to figure out how to close the shared semaphor.
*	deamonize on startup