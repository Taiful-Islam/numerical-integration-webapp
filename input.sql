CREATE TABLE students (id INT, name TEXT, department TEXT, cgpa FLOAT);
INSERT INTO students VALUES (1, 'Taiful', 'CSE', 3.50);
INSERT INTO students VALUES (2, 'Rahim', 'CSE', 3.20);
INSERT INTO students VALUES (3, 'Karim', 'EEE', 3.70);
SELECT * FROM students;
SELECT name, cgpa FROM students WHERE department = 'CSE';
