
# Evolutionary algorithms for puredata

- version of pd I'm running is 0.48.1

========
# Project setup

- Each external is written in a single C file with a single corresponding makefile. Each external is in its own folder
- There's a directory called pd-lib-builder which is a direct copy of the same folder in https://github.com/pure-data/externals-howto which hides all the complexities of the build. The makefile in each folder accesses this. 
- **Unit Testing** First thing I'm trying is catch2: https://github.com/catchorg/Catch2 as it is supposed to be the simplest. To integrate with travis-ci, I folowed https://github.com/justiceadamsUNI/CPP-Unit-Testing-Suite-With-CI




=========
# Externals documentation

## 1: Hellopdworld





========
# Notes on Writing C programs for pd

pd is supposed to be easy to extend via "externals"

## Writing the program 

There's a few good guides for this. The one I'm doing is here: 

https://github.com/pure-data/externals-howto

It's really good, but it *doesn't*  give details about what to do after the program compiles. The folder `howto` in this project builds on these projects with a little more info about using them 

## Compiling the program

- https://github.com/pure-data/pd-lib-builder

## Installing

This is simply a case of copying or moving the compiled files to the externals directory that pd is using. 


- https://forum.pdpatchrepo.info/topic/6743/how-to-install-externals-libraries-gui-plug-ins-etc
- http://puredata.info/docs/faq/how-do-i-install-externals-and-help-files

## Using

*Once I'd written, compiled and installed a library, it **still** wasn't clear how they'd appear!*





## pd titbits

https://puredata.info/
