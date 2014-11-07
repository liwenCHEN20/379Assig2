COMPUT 379 ASSIG 2
==================

This is the cmput 379 assignment 2 for Fall 2014. An simple webserver implementation. If you are a UofA student using this code, please don't hand it in as your own.
Use as a reference only.

Server_T:
---------

Using posix threads to service requests.

Handing in with the following remaining

Need to do:
*	Change attributes to start in kernel detached mode.

Server_F:
---------

Fully complete


LIMITATIONS:
------------
*	Needs Signal handling
*	No memory leak testing. There is at least 1 known
*	Code could definitely be cleaned up and sepparated into specific files better
*	Parsing commandline arguments is kind of fragile.
*	Parsing http header is kind of fragile
	*	a severly malformed header could cause problems 
*	SEVERE lack of error handling for things like opening files, requesting memory
	*	Lack of error handling in general

*	Critical Security Issue: could pass malicious path in http header
	server would  attempt to open it