#include <stdio.h>

int yyparse(void);

int main(void)
{
    printf("Mini SQL Engine\n");
    printf("Enter SQL commands.\n");
    printf("Press Ctrl+Z and Enter to exit.\n\n");

    return yyparse();
}
