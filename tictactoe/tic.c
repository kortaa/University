#include "tic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char square[10];


void board()
{
    system("cls");
    printf("     |     |     \n");
    printf("  %c  |  %c  |  %c  \n", square[0], square[1], square[2]);
    printf("     |     |     \n");

    printf("-----|-----|-----\n");

    printf("     |     |     \n");
    printf("  %c  |  %c  |  %c  \n", square[3], square[4], square[5]);
    printf("     |     |     \n");

    printf("-----|-----|-----\n");

    printf("     |     |     \n");
    printf("  %c  |  %c  |  %c  \n", square[6], square[7], square[8]);
    printf("     |     |     \n");

    printf("\n\n\n");
    printf("Wpisz liczby (1-9): ");
}
void zerowanie()
{
    for(int i = 0; i < 9; i++) square[i] = ' ';
}

void input()
{
    int where;

    scanf("%d", &where);

    where--;

    if(where > 9) printf("Error: Bledny ruch\n");

    if(square[where] == ' ')
    {
        square[where] = 'x';
    }

    else
    {
        system("cls");
        printf("Error: Bledny ruch\n");
        sleep(1);
        board();
        input();
    }

}
void computer()
{
    srand(time(NULL));

    int where;
    int flag = 0;

    while(!flag)
    {
        where = rand() % 9;
        if(square[where] == ' ')
        {
            square[where] = 'o';
            flag = 1;
        }
    }
}

int end()
{
    for(int i = 0; i < 3; i+=3) //wiersze
    {
        if(square[i] == 'x' && square[i] == square[i+1] && square[i] == square[i+2])
        {
            system("cls");
            printf("Wygrales\n");
            return 1;
        }
    }

    for(int i = 0; i < 3; i++) //kolumny
    {
        if(square[i] == 'x' && square[i] == square[i+3] && square[i] == square[i+6])
        {
            system("cls");
            printf("Wygrales\n");
            return 1;
        }
    }

    if(square[0] == 'x' && square[4] == 'x' && square[8] == 'x') //pierwsza przekatna
    {
        system("cls");
        printf("Wygrales\n");
        return 1;
    }

    if(square[2] == 'x' && square[4] == 'x' && square[6] == 'x') //druga przekatna
    {
        system("cls");
        printf("Wygrales\n");
        return 1;
    }

    for(int i = 0; i < 3; i+=3) //wiersze
    {
        if(square[i] == 'o' && square[i] == square[i+1] && square[i] == square[i+2])
        {
            system("cls");
            printf("Przegrales\n");
            return 1;
        }
    }

    for(int i = 0; i < 3; i++) //kolumny
    {
        if(square[i] == 'o' && square[i] == square[i+3] && square[i] == square[i+6])
        {
            system("cls");
            printf("przegrales\n");
            return 1;
        }
    }

    if(square[0] == 'o' && square[4] == 'o' && square[8] == 'o') //pierwsza przekatna
    {
        system("cls");
        printf("Przegrales\n");
        return 1;
    }

    if(square[2] == 'o' && square[4] == 'o' && square[6] == 'o') //druga przekatna
    {
        system("cls");
        printf("Przegrales\n");
        return 1;
    }

    return 0;

}


