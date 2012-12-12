#include <stdio.h>
#include <stdlib.h>

int main()
{
    int worteins = 0;
    int wortzwei = 6;
    int dummy1 = 0;
    for(int j = 0; j<=15; j++)
    {
        for(int i = 0; i<=7;i++)
        {
            dummy1 ^= ((worteins>>i) & 0x01);
        }
        printf("Zahl: %d  und Paritaet: %d\n", worteins, dummy1);
        worteins++;
        dummy1 = 0;
    }
    worteins = 0;
    for(int i =0; i<=7; i++)
    {
        dummy1 |= ((wortzwei>>i) & 0x01);
        dummy1 = (dummy1<<i);
        worteins |= dummy1;
    }
    printf("Resultat: %d", worteins);

    return 0;
}
