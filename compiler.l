%{
#include "compiler.tab.h"
#include <string.h>
#include <stdlib.h>
%}

%option noyywrap
%option caseless

ID      [a-zA-Z_][a-zA-Z0-9_]*
NUMBER  [0-9]+(\.[0-9]+)?
STRING  \'([^\'\n]|\\\')*\'

%%

[ \t\r\n]+                  ;

"CREATE"                    return CREATE;
"TABLE"                     return TABLE;
"INSERT"                    return INSERT;
"INTO"                      return INTO;
"VALUES"                    return VALUES;
"SELECT"                    return SELECT;
"FROM"                      return FROM;
"WHERE"                     return WHERE;
"UPDATE"                    return UPDATE;
"SET"                       return SET;
"DELETE"                    return DELETE;
"DROP"                      return DROP;
"SHOW"                      return SHOW;
"TABLES"                    return TABLES;
"DESCRIBE"                  return DESCRIBE;

"INT"                       {
                                yylval.str = _strdup("INT");
                                return TYPE;
                            }

"FLOAT"                     {
                                yylval.str = _strdup("FLOAT");
                                return TYPE;
                            }

"TEXT"                      {
                                yylval.str = _strdup("TEXT");
                                return TYPE;
                            }

"*"                         return STAR;
","                         return COMMA;
"("                         return LPAREN;
")"                         return RPAREN;
"="                         return EQUAL;
";"                         return SEMICOLON;

{STRING}                    {
                                int length = yyleng;
                                char *value;

                                value = (char *)malloc(length - 1);

                                if (value == NULL)
                                    exit(1);

                                strncpy(
                                    value,
                                    yytext + 1,
                                    length - 2
                                );

                                value[length - 2] = '\0';

                                yylval.str = value;

                                return STRING_LITERAL;
                            }

{NUMBER}                    {
                                yylval.str = _strdup(yytext);
                                return NUMBER;
                            }

{ID}                        {
                                yylval.str = _strdup(yytext);
                                return IDENTIFIER;
                            }

.                           {
                                return yytext[0];
                            }

%%