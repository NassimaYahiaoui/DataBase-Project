  Nassima Yahiaoui
  Professor: Liz. Miller
  Course: CIT 284                                                                                                                               
  April 15, 2019 


## Features
- Support three data types: int, float and char(n) where 1 ≤ n ≤ 255
- Support tables with up to 32 attributes. Support primary key and unique key definition.
- Support indices on unique keys.
- Support six operations for selection and deletion: =, <>, <, >, <= and >=. Operation = can be accelerated by existing indices.
- Support the following instructions:
    - select
    - insert
    - delete
    - create table / index
    - drop table / index
    - exec / execfile (Execute a .sql file)
    - exit / quit

## Example
```
DBMS> create database student;
1 database created. Query done in 0s.
DBMS> use student;
Database changed.
DBMS> use teachers;
Database teachers does not exist. Query done in 0s.
DBMS> create table student (
	...>   student_id char(8),
    ...>   first_name char(8),
    ...>   lat_name char(16) unique,
    ...>   age int,
    ...>   gender char(1),
    ...>   primary key(student_id)
    ...> );
1 table created. Query done in 0s.

DBMS> insert into student values (00001', 'Alice', 'Jung',17, 'F');
1 record inserted. Query done in 0s.

DBMS> insert into student values ('00002', 'Bob', 'Dennis',13, 'M');
1 record inserted. Query done in 0s.

DBMS> insert into student values ('00006', 'Ralo', 'Dennis',13, 'M');
ERROR: [RecordManager::insert] Duplicate values in unique column `first_name` of table `student`!

DBMS> insert into student values ('00004', 'Cindy', 'Deco',25, 'F');
1 record inserted. Query done in 0s.

DBMS> insert into student values ('00005', 'Dave', 'Steve', 16, 'M');
1 record inserted. Query done in 0s.

DBMS> select * from student;

studentid     first_name   lat_name    age		gender
------------------------------------------------------
00001   		Alice   	Jung      	17		F
00002   		Bob     	Dennis      13		M
00004   		Cindy   	Deco      	25		F
00005   		Dave    	Steve      	16		M

4 record(s) selected. Query done in 0s.

DBMS> select * from student where student_id = '00002';

student_id     first_name   lat_name    age		gender
------------------------------------------------------
00002   		Bob     	Dennis      13		M

1 record(s) selected. Query done in 0s.

DBMS> select * from student where age >= 15 and age <= 25;

studentid     first_name   lat_name    age		gender
------------------------------------------------------
00001   		Alice   	Jung      	17		F
00002   		Bob     	Dennis      13		M
00004   		Cindy   	Deco      	25		F
00005   		Dave    	Steve      	16		M

3 record(s) selected. Query done in 0.016s.

DBMS> delete from student where gender <> 'M';
2 record(s) deleted. Query done in 0s.

DBMS> select * from student;

studentid     first_name   lat_name    age		gender
------------------------------------------------------
00002   		Bob     	Dennis      13		M
00005   		Dave    	Steve      	16		M

2 record(s) selected. Query done in 0.016s.

DBMS> drop table student;
1 table dropped. Query done in 0.016s.

DBMS> exit;
Bye~ :)
```
Make file is used to compile the program in windows without using compiler
Database is stored in binary file indices.
Tables are save in data folder
