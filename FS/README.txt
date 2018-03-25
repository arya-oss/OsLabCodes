=======================================
	Portable File System
Author: Rajmani Arya
Date: 10-Jun-2017
=======================================

compile: gcc -o PFS.exe PFS.c
run (Command Prompt): ./PFS.exe
or can be compiled directly from any C/C++ IDE.

Assumptions:

Data Structure are defined for SuperBlock (it contains summary of file system)
INode and File Control Block are same (vary on different os). for each file or 
directory a new INode is allocated.

At "open PFSFile" command, this program created a new file of size 10 KBytes and
writes SuperBlock and Root Directory.

Free Data Block is managed by Bitmap arrays.

DISK SIZE = 10240 Bytes
Super Block Size = 1 Block = 256 Bytes
INode's Block Size = 15 Blocks = 3840 Bytes
Data Block Size = 24 Blocks = 6144 Bytes

Features:
   1. Portable to any OS (having C/C++ environment)
   
Limitations:
   1. max file store capacity is 6 KB since data block capacity is 6144 bytes.
   2. Nested directory is not supported. (cd, mkdir, pwd)
   3. for a specific command required no. of arguments must be present otherwise program may crash.
   4. for large file storage, linked files e.g pfs.1, pfs.2 etc. is NOT supported.
