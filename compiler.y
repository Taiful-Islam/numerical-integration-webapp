%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"

int yylex(void);
void yyerror(const char *message);

char *copy_string(const char *text)
{
    if (text == NULL)
        return NULL;

    return _strdup(text);
}
%}


/* -----------------------------
   Semantic Value
   ----------------------------- */

%union
{
    char *str;
}


/* -----------------------------
   SQL Keywords
   ----------------------------- */

%token CREATE
%token TABLE
%token INSERT
%token INTO
%token VALUES
%token SELECT
%token FROM
%token WHERE
%token UPDATE
%token SET
%token DELETE
%token DROP
%token SHOW
%token TABLES
%token DESCRIBE


/* -----------------------------
   Tokens with string values
   IMPORTANT: TYPE must have <str>
   ----------------------------- */

%token <str> TYPE
%token <str> IDENTIFIER
%token <str> NUMBER
%token <str> STRING_LITERAL


/* -----------------------------
   Symbols
   ----------------------------- */

%token STAR
%token COMMA
%token LPAREN
%token RPAREN
%token EQUAL
%token SEMICOLON


/* -----------------------------
   Non-terminal types
   ----------------------------- */

%type <str> column_defs
%type <str> column_def_list
%type <str> column_def
%type <str> type_name

%type <str> value_list
%type <str> value
%type <str> select_list


%%


/* =========================================================
   PROGRAM
   ========================================================= */

program:
        statements
        ;


statements:
        statements statement
        | statement
        ;


/* =========================================================
   STATEMENTS
   ========================================================= */

statement:
        create_statement
        | insert_statement
        | select_statement
        | update_statement
        | delete_statement
        | drop_statement
        | show_statement
        | describe_statement

        | error SEMICOLON
        {
            yyerrok;
        }
        ;


/* =========================================================
   CREATE TABLE
   ========================================================= */

create_statement:
        CREATE TABLE IDENTIFIER
        LPAREN column_defs RPAREN SEMICOLON
        {
            char *columns[64];
            char *types[64];

            int count = 0;

            char *part;
            char *separator;

            part = strtok($5, "|");

            while (part != NULL && count < 64)
            {
                separator = strchr(part, ':');

                if (separator != NULL)
                {
                    *separator = '\0';

                    columns[count] = part;
                    types[count] = separator + 1;

                    count++;
                }

                part = strtok(NULL, "|");
            }

            execute_create(
                $3,
                columns,
                types,
                count
            );

            free($3);
            free($5);
        }
        ;


/* =========================================================
   COLUMN DEFINITIONS
   ========================================================= */

column_defs:
        column_def_list
        {
            $$ = $1;
        }
        ;


column_def_list:
        column_def
        {
            $$ = copy_string($1);

            free($1);
        }

        | column_def_list COMMA column_def
        {
            int size;

            size = strlen($1)
                 + strlen($3)
                 + 2;

            $$ = (char *)malloc(size);

            if ($$ == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }

            sprintf(
                $$,
                "%s|%s",
                $1,
                $3
            );

            free($1);
            free($3);
        }
        ;


/* =========================================================
   SINGLE COLUMN
   ========================================================= */

column_def:
        IDENTIFIER type_name
        {
            int size;

            size = strlen($1)
                 + strlen($2)
                 + 2;

            $$ = (char *)malloc(size);

            if ($$ == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }

            sprintf(
                $$,
                "%s:%s",
                $1,
                $2
            );

            free($1);
            free($2);
        }
        ;


/* =========================================================
   DATA TYPES
   ========================================================= */

type_name:
        TYPE
        {
            $$ = $1;
        }
        ;


/* =========================================================
   INSERT
   ========================================================= */

insert_statement:
        INSERT INTO IDENTIFIER
        VALUES LPAREN value_list RPAREN SEMICOLON
        {
            char *values[64];

            int count = 0;

            char *part;

            part = strtok($6, "|");

            while (part != NULL && count < 64)
            {
                values[count] = part;

                count++;

                part = strtok(NULL, "|");
            }

            execute_insert(
                $3,
                values,
                count
            );

            free($3);
            free($6);
        }
        ;


/* =========================================================
   VALUE LIST
   ========================================================= */

value_list:
        value
        {
            $$ = copy_string($1);

            free($1);
        }

        | value_list COMMA value
        {
            int size;

            size = strlen($1)
                 + strlen($3)
                 + 2;

            $$ = (char *)malloc(size);

            if ($$ == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }

            sprintf(
                $$,
                "%s|%s",
                $1,
                $3
            );

            free($1);
            free($3);
        }
        ;


/* =========================================================
   VALUE
   ========================================================= */

value:
        NUMBER
        {
            $$ = $1;
        }

        | STRING_LITERAL
        {
            $$ = $1;
        }

        | IDENTIFIER
        {
            $$ = $1;
        }
        ;


/* =========================================================
   SELECT
   ========================================================= */

select_statement:

        /* -------------------------
           SELECT without WHERE
           ------------------------- */

        SELECT select_list
        FROM IDENTIFIER
        SEMICOLON
        {
            char *columns[64];

            int count = 0;

            char *part;

            part = strtok($2, "|");

            while (part != NULL && count < 64)
            {
                columns[count] = part;

                count++;

                part = strtok(NULL, "|");
            }

            execute_select(
                $4,
                columns,
                count,
                NULL,
                NULL
            );

            free($2);
            free($4);
        }


        /* -------------------------
           SELECT with WHERE
           ------------------------- */

        | SELECT select_list
          FROM IDENTIFIER
          WHERE IDENTIFIER EQUAL value
          SEMICOLON
        {
            char *columns[64];

            int count = 0;

            char *part;

            part = strtok($2, "|");

            while (part != NULL && count < 64)
            {
                columns[count] = part;

                count++;

                part = strtok(NULL, "|");
            }

            execute_select(
                $4,
                columns,
                count,
                $6,
                $8
            );

            free($2);
            free($4);
            free($6);
            free($8);
        }
        ;


/* =========================================================
   SELECT COLUMN LIST
   ========================================================= */

select_list:

        STAR
        {
            $$ = copy_string("*");
        }

        | IDENTIFIER
        {
            $$ = $1;
        }

        | select_list COMMA IDENTIFIER
        {
            int size;

            size = strlen($1)
                 + strlen($3)
                 + 2;

            $$ = (char *)malloc(size);

            if ($$ == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }

            sprintf(
                $$,
                "%s|%s",
                $1,
                $3
            );

            free($1);
            free($3);
        }
        ;


/* =========================================================
   UPDATE
   ========================================================= */

update_statement:
        UPDATE IDENTIFIER
        SET IDENTIFIER EQUAL value
        WHERE IDENTIFIER EQUAL value
        SEMICOLON
        {
            execute_update(
                $2,
                $4,
                $6,
                $8,
                $10
            );

            free($2);
            free($4);
            free($6);
            free($8);
            free($10);
        }
        ;


/* =========================================================
   DELETE
   ========================================================= */

delete_statement:
        DELETE FROM IDENTIFIER
        WHERE IDENTIFIER EQUAL value
        SEMICOLON
        {
            execute_delete(
                $3,
                $5,
                $7
            );

            free($3);
            free($5);
            free($7);
        }
        ;


/* =========================================================
   DROP TABLE
   ========================================================= */

drop_statement:
        DROP TABLE IDENTIFIER
        SEMICOLON
        {
            execute_drop($3);

            free($3);
        }
        ;


/* =========================================================
   SHOW TABLES
   ========================================================= */

show_statement:
        SHOW TABLES SEMICOLON
        {
            execute_show_tables();
        }
        ;


/* =========================================================
   DESCRIBE TABLE
   ========================================================= */

describe_statement:
        DESCRIBE IDENTIFIER
        SEMICOLON
        {
            execute_describe($2);

            free($2);
        }
        ;


%%


/* =========================================================
   ERROR HANDLER
   ========================================================= */

void yyerror(const char *message)
{
    fprintf(
        stderr,
        "SQL ERROR: %s\n",
        message
    );
}