#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include "engine.h"

#define DATA_FOLDER "data"
#define MAX_LINE 8192
#define MAX_COLUMNS 64
#define MAX_NAME 128

void make_path(char *path, const char *table)
{
    sprintf(path, "%s/%s.csv", DATA_FOLDER, table);
}

void make_data_folder()
{
    _mkdir(DATA_FOLDER);
}

int split_line(char *line, char **parts)
{
    int count = 0;
    char *start = line;
    int quote = 0;
    char *p = line;

    while (*p != '\0')
    {
        if (*p == '\'')
        {
            quote = !quote;
        }
        else if (*p == ',' && quote == 0)
        {
            *p = '\0';
            parts[count] = start;
            count++;
            start = p + 1;
        }

        p++;
    }

    parts[count] = start;
    count++;

    return count;
}

void clean_value(char *value)
{
    int length = strlen(value);

    while (length > 0 &&
           (value[length - 1] == '\n' ||
            value[length - 1] == '\r' ||
            value[length - 1] == ' ' ||
            value[length - 1] == '\t'))
    {
        value[length - 1] = '\0';
        length--;
    }

    while (*value == ' ' || *value == '\t')
        value++;

    length = strlen(value);

    if (length >= 2 && value[0] == '\'' && value[length - 1] == '\'')
    {
        value[length - 1] = '\0';
        memmove(value, value + 1, length - 1);
    }
}

int get_columns(const char *table, char names[][MAX_NAME],
                char types[][32])
{
    char path[256];
    char line[MAX_LINE];

    make_path(path, table);

    FILE *file = fopen(path, "r");

    if (file == NULL)
        return 0;

    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return 0;
    }

    char *parts[MAX_COLUMNS];
    int count = split_line(line, parts);

    int i;

    for (i = 0; i < count; i++)
    {
        char *separator = strchr(parts[i], ':');

        if (separator != NULL)
        {
            *separator = '\0';

            strcpy(names[i], parts[i]);
            strcpy(types[i], separator + 1);
        }
        else
        {
            strcpy(names[i], parts[i]);
            strcpy(types[i], "TEXT");
        }

        clean_value(names[i]);
        clean_value(types[i]);
    }

    fclose(file);

    return count;
}

int find_column(char names[][MAX_NAME], int count, const char *name)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (_stricmp(names[i], name) == 0)
            return i;
    }

    return -1;
}

int value_equal(const char *a, const char *b)
{
    return _stricmp(a, b) == 0;
}

void execute_create(char *table, char **columns, char **types, int count)
{
    char path[256];
    int i;

    make_data_folder();
    make_path(path, table);

    FILE *check = fopen(path, "r");

    if (check != NULL)
    {
        fclose(check);
        printf("ERROR: Table '%s' already exists.\n", table);
        return;
    }

    FILE *file = fopen(path, "w");

    if (file == NULL)
    {
        printf("ERROR: Could not create table '%s'.\n", table);
        return;
    }

    for (i = 0; i < count; i++)
    {
        if (i > 0)
            fprintf(file, ",");

        fprintf(file, "%s:%s", columns[i], types[i]);
    }

    fprintf(file, "\n");
    fclose(file);

    printf("SUCCESS: Table '%s' created.\n", table);
}

void execute_insert(char *table, char **values, int count)
{
    char path[256];
    char names[MAX_COLUMNS][MAX_NAME];
    char types[MAX_COLUMNS][32];

    make_data_folder();
    make_path(path, table);

    int columns = get_columns(table, names, types);

    if (columns == 0)
    {
        printf("ERROR: Table '%s' does not exist.\n", table);
        return;
    }

    if (columns != count)
    {
        printf("ERROR: Table has %d columns but %d values were given.\n",
               columns, count);
        return;
    }

    FILE *file = fopen(path, "a");

    if (file == NULL)
    {
        printf("ERROR: Could not open table.\n");
        return;
    }

    int i;

    for (i = 0; i < count; i++)
    {
        if (i > 0)
            fprintf(file, ",");

        fprintf(file, "%s", values[i]);
    }

    fprintf(file, "\n");
    fclose(file);

    printf("SUCCESS: 1 row inserted into '%s'.\n", table);
}

void execute_select(char *table, char **columns, int column_count,
                    char *where_column, char *where_value)
{
    char path[256];
    char names[MAX_COLUMNS][MAX_NAME];
    char types[MAX_COLUMNS][32];
    char line[MAX_LINE];

    make_path(path, table);

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        printf("ERROR: Table '%s' does not exist.\n", table);
        return;
    }

    int total_columns = get_columns(table, names, types);

    int selected[MAX_COLUMNS];
    int selected_count = 0;

    if (column_count == 1 && strcmp(columns[0], "*") == 0)
    {
        int i;

        for (i = 0; i < total_columns; i++)
        {
            selected[selected_count] = i;
            selected_count++;
        }
    }
    else
    {
        int i;

        for (i = 0; i < column_count; i++)
        {
            int index = find_column(names, total_columns, columns[i]);

            if (index == -1)
            {
                printf("ERROR: Unknown column '%s'.\n", columns[i]);
                fclose(file);
                return;
            }

            selected[selected_count] = index;
            selected_count++;
        }
    }

    int where_index = -1;

    if (where_column != NULL)
    {
        where_index = find_column(names, total_columns, where_column);

        if (where_index == -1)
        {
            printf("ERROR: Unknown WHERE column '%s'.\n", where_column);
            fclose(file);
            return;
        }
    }

    printf("\n");
    printf("QUERY RESULT\n");
    printf("---------------------------------------------\n");

    int i;

    for (i = 0; i < selected_count; i++)
    {
        if (i > 0)
            printf(" | ");

        printf("%-15s", names[selected[i]]);
    }

    printf("\n");

    for (i = 0; i < selected_count; i++)
        printf("----------------");

    printf("\n");

    fgets(line, sizeof(line), file);

    int rows = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char copy[MAX_LINE];
        strcpy(copy, line);

        char *parts[MAX_COLUMNS];
        int count = split_line(copy, parts);

        for (i = 0; i < count; i++)
            clean_value(parts[i]);

        if (where_index != -1)
        {
            if (where_index >= count ||
                !value_equal(parts[where_index], where_value))
            {
                continue;
            }
        }

        int j;

        for (j = 0; j < selected_count; j++)
        {
            if (j > 0)
                printf(" | ");

            printf("%-15s", parts[selected[j]]);
        }

        printf("\n");
        rows++;
    }

    printf("---------------------------------------------\n");
    printf("%d row(s) returned.\n\n", rows);

    fclose(file);
}

void execute_update(char *table, char *set_column, char *set_value,
                    char *where_column, char *where_value)
{
    char path[256];
    char temp_path[256];
    char names[MAX_COLUMNS][MAX_NAME];
    char types[MAX_COLUMNS][32];
    char line[MAX_LINE];

    make_path(path, table);
    sprintf(temp_path, "%s/__temp.csv", DATA_FOLDER);

    FILE *input = fopen(path, "r");

    if (input == NULL)
    {
        printf("ERROR: Table '%s' does not exist.\n", table);
        return;
    }

    int column_count = get_columns(table, names, types);

    int set_index = find_column(names, column_count, set_column);
    int where_index = find_column(names, column_count, where_column);

    if (set_index == -1 || where_index == -1)
    {
        printf("ERROR: Unknown column in UPDATE.\n");
        fclose(input);
        return;
    }

    FILE *output = fopen(temp_path, "w");

    if (output == NULL)
    {
        fclose(input);
        printf("ERROR: Could not create temporary file.\n");
        return;
    }

    fgets(line, sizeof(line), input);
    fputs(line, output);

    int updated = 0;

    while (fgets(line, sizeof(line), input) != NULL)
    {
        char copy[MAX_LINE];
        strcpy(copy, line);

        char *parts[MAX_COLUMNS];
        int count = split_line(copy, parts);

        int i;

        for (i = 0; i < count; i++)
            clean_value(parts[i]);

        if (where_index < count &&
            value_equal(parts[where_index], where_value))
        {
            strcpy(parts[set_index], set_value);
            updated++;
        }

        for (i = 0; i < count; i++)
        {
            if (i > 0)
                fprintf(output, ",");

            fprintf(output, "%s", parts[i]);
        }

        fprintf(output, "\n");
    }

    fclose(input);
    fclose(output);

    remove(path);
    rename(temp_path, path);

    printf("SUCCESS: %d row(s) updated.\n", updated);
}

void execute_delete(char *table, char *where_column, char *where_value)
{
    char path[256];
    char temp_path[256];
    char names[MAX_COLUMNS][MAX_NAME];
    char types[MAX_COLUMNS][32];
    char line[MAX_LINE];

    make_path(path, table);
    sprintf(temp_path, "%s/__temp.csv", DATA_FOLDER);

    FILE *input = fopen(path, "r");

    if (input == NULL)
    {
        printf("ERROR: Table '%s' does not exist.\n", table);
        return;
    }

    int column_count = get_columns(table, names, types);
    int where_index = find_column(names, column_count, where_column);

    if (where_index == -1)
    {
        printf("ERROR: Unknown column '%s'.\n", where_column);
        fclose(input);
        return;
    }

    FILE *output = fopen(temp_path, "w");

    if (output == NULL)
    {
        fclose(input);
        return;
    }

    fgets(line, sizeof(line), input);
    fputs(line, output);

    int deleted = 0;

    while (fgets(line, sizeof(line), input) != NULL)
    {
        char copy[MAX_LINE];
        strcpy(copy, line);

        char *parts[MAX_COLUMNS];
        int count = split_line(copy, parts);

        int i;

        for (i = 0; i < count; i++)
            clean_value(parts[i]);

        if (where_index < count &&
            value_equal(parts[where_index], where_value))
        {
            deleted++;
            continue;
        }

        fputs(line, output);
    }

    fclose(input);
    fclose(output);

    remove(path);
    rename(temp_path, path);

    printf("SUCCESS: %d row(s) deleted.\n", deleted);
}

void execute_drop(char *table)
{
    char path[256];

    make_path(path, table);

    if (remove(path) == 0)
        printf("SUCCESS: Table '%s' dropped.\n", table);
    else
        printf("ERROR: Table '%s' does not exist.\n", table);
}

void execute_show_tables(void)
{
    struct _finddata_t file;
    intptr_t handle;

    make_data_folder();

    printf("\nTABLES\n");
    printf("-------------------------\n");

    handle = _findfirst("data/*.csv", &file);

    if (handle == -1)
    {
        printf("No tables found.\n\n");
        return;
    }

    int count = 0;

    do
    {
        char name[256];
        strcpy(name, file.name);

        char *extension = strrchr(name, '.');

        if (extension != NULL)
            *extension = '\0';

        if (strcmp(name, "__temp") != 0)
        {
            printf("%s\n", name);
            count++;
        }

    } while (_findnext(handle, &file) == 0);

    _findclose(handle);

    printf("-------------------------\n");
    printf("%d table(s).\n\n", count);
}

void execute_describe(char *table)
{
    char names[MAX_COLUMNS][MAX_NAME];
    char types[MAX_COLUMNS][32];

    int count = get_columns(table, names, types);

    if (count == 0)
    {
        printf("ERROR: Table '%s' does not exist.\n", table);
        return;
    }

    printf("\nTABLE: %s\n", table);
    printf("-------------------------------\n");
    printf("%-20s %-10s\n", "COLUMN", "TYPE");
    printf("-------------------------------\n");

    int i;

    for (i = 0; i < count; i++)
        printf("%-20s %-10s\n", names[i], types[i]);

    printf("-------------------------------\n\n");
}
