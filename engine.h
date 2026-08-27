#ifndef ENGINE_H
#define ENGINE_H

void execute_create(char *table, char **columns, char **types, int count);
void execute_insert(char *table, char **values, int count);
void execute_select(char *table, char **columns, int column_count,
                    char *where_column, char *where_value);
void execute_update(char *table, char *set_column, char *set_value,
                    char *where_column, char *where_value);
void execute_delete(char *table, char *where_column, char *where_value);
void execute_drop(char *table);
void execute_show_tables(void);
void execute_describe(char *table);

#endif
