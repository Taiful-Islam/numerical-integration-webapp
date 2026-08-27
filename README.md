# Professional Mini SQL Engine

University Compiler Design Project

This project demonstrates a small SQL processing system built with:

- Flex - lexical analysis
- Bison - syntax analysis
- C - query execution and file-based storage
- Python Tkinter - desktop GUI

## Project flow

SQL Query
   |
   v
Flex Lexer
   |
   v
Bison Parser
   |
   v
C Query Engine
   |
   v
CSV Table Storage

## Supported commands

CREATE TABLE
INSERT INTO ... VALUES
SELECT *
SELECT selected columns
SELECT ... WHERE column = value
UPDATE ... SET ... WHERE
DELETE ... WHERE
DROP TABLE
SHOW TABLES
DESCRIBE table

## Windows requirements

Check these commands in VS Code PowerShell:

    win_flex --version
    win_bison --version
    gcc --version
    python --version

## Build

Open the project folder in VS Code.

Run:

    .\build.bat

Then:

    python app.py

or simply:

    .\run.bat

## Example

    CREATE TABLE students (id INT, name TEXT, department TEXT, cgpa FLOAT);
    INSERT INTO students VALUES (1, 'Taiful', 'CSE', 3.50);
    INSERT INTO students VALUES (2, 'Rahim', 'CSE', 3.20);
    SELECT * FROM students;
    SELECT name, cgpa FROM students WHERE department = 'CSE';
    UPDATE students SET cgpa = 3.60 WHERE id = 1;
    DELETE FROM students WHERE id = 2;
    SHOW TABLES;
    DESCRIBE students;

## Storage

Every table is stored as a CSV file inside the data folder.

This is an educational project and is not intended to replace MySQL, PostgreSQL, or another production database.
