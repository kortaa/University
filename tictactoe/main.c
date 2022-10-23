#include "tic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main()
{
    zerowanie();
    board();
    int n = 0;
    while(!end() && n < 5)
    {
        input();
        board();
        if( n < 4) computer();
        board();
        n++;
    }

    if(!end())
    {
        system("cls");
        printf("Remis\n");
    }

    return 0;
}
