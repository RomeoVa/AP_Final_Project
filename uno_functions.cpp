#include "uno_functions.h"

using namespace std;


pair <int, int> get_card()
{
    pair <int, int> card ; 
    int number;
    int color;
    
    if (wild_card())
    {
        cout << "WILD CARD ";
        number = -1;
        color = 0;
    }
    else
    {
        cout << "NORMAL CARD ";
        number = rand() % 13 ;  // random number between 0-12
        color = rand() % 4 + 1 ;  // random number between 1-4
    }
    card.first = number; 
    card.second = color ; 

    return card;
}

bool wild_card()
{
    int probability;
    probability = rand() % 99 ;
    if (probability < 10)
    {
        return true;
    }
    else
    {
        return false;
    }
}
