## Formal Relational Query Parser

FRQP is a C++ implementation of the Relational Query language used in DBMS. Relational algebra is a procedural query language which consists of a set of operations that take one or two relations as input and produce a new relation as the output. The fundamental operations in the relational algebra are *select*, *project*, *union*, *set difference*, *cartesian product* and *rename*.

### CONTENTS

* Requirements
* Quick Start
* Documentation
* Package Contents

### REQUIREMENTS

Operating systems: Linux, Fedora, Windows

Tested compilers: gcc/g++ 4.8.4 under Linux

For gcc/g++ tool chain, GNU make is required for building.


### QUICK START

For Linux, at the top-level directory, run the following commands:

```
$ g++ main.cpp -o main.o
$ ./main.o
```

Please read **examples/queries.txt** for sample queries.
 
* Try simple queries which carry out just one RA operation.They will follow the following syntax:
```
operation_name(table_name) args
```

For instance,
```
>> SELECT(table) condition1^condition2^......
>> PROJECT(table) attribute1,attribute2,.....
>> RENAME(table) new_name 
>> RENAME(table_name) new_name{new names of all attributes}
>> UNION(table1) [table2]
>> SETDIFFERENCE(table1) [table2]
>> CARTESIANPRODUCT(table1) [table2]
```

* Try nested queries which carry out multiple RA operations. They will follow the following syntax:
```
operation1(operation2(......(table_name) .......) args2) args1
```

All the other RA operations can be done by nesting the six fundamental operations.

For instance,

1. INTERSECT(table1,table2):	
```
>> SETDIFFERENCE(table1) [SETDIFFERENCE(table1) [table2]]
```

2. DIVIDE(relation,table1):
```
>> PROJECT(SETDIFFERENCE(CARTESIANPRODUCT(table1) [table2]) [relation]) args
```

* For exit, just type EXIT in the query.
```
>>> EXIT
```

### DOCUMENTATION

Documentation can be found in the **doc** directory. 

See **PACKAGE CONTENTS** for a detailed listing.


### PACKAGE CONTENTS
| <!-- -->              | <!-- -->                  |
| -------------         |:-------------:            |
| README.md             | This file                 |
| doc/Documentation.pdf | Formal Documentation      |
| examples/queries.txt  | Sample queries text file  |
|res/                   |db dump                    |
|src/                   |source codes               |

