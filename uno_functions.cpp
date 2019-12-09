/*
    UNO game functions
    
    Created  by
    Mauricio Peón García        A01024162
    Pablo Terán Ríos            A01421434
    Romeo Varela Nagore         A01020736

    07/12/2019
*/

#include "uno_functions.h"
#include "colors.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <map>
#include <cstring>

//***************************************
// card.first = number;
// card.second = color;
//***************************************

using namespace std;
#define BUFFER_SIZE 1024

//Get a UNO game card
pair <int, int> get_card()
{
    pair <int, int> card ;
    int number;
    int color;
    if (wild_card())
    {
        number = rand() % 2 + 1;  // random number between 1 - 2
        number *= -1;
        color = 0;
    }
    else
    {
        number = rand() % 13 ;  // random number between 0-12
        color = rand() % 4 + 1 ;  // random number between 1-4
    }
    card.first = number;
    card.second = color;

    return card;
}
// Get wild card probability for the UNO game. 
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
// Save initial hand
vector<pair <int, int> > player_hand()
{
    vector<pair <int, int> > hand;
    for (int i = 0; i < 7; i++)
    {
        hand.push_back(get_card());
    }
    return hand;
}
// Check if the card given by the player can be played
bool verifyCard(pair <int,int> * card,pair <int,int> * deck_card)
{
    if(card->first == deck_card->first || card->second == deck_card->second)
    {
        return true;
    }
    else if (card->first == -1 || card->first == -2) // WILD CARD
    {
        return true;
    }
    return false;
}

// Get a card color given two integers
string getColor(pair <int,int> * card)
{
    switch (card->second)
    {
        case BLACK: return "Black";
        case RED: return "Red";
        case BLUE: return "Blue";
        case GREEN: return "Green";
        case YELLOW: return "Yellow";
        default: return "Invalid color";
    }
}
// Delete a card from a hand at a given position
void deleteCardAtPosition(vector<pair <int, int> > * hand,int position)
{
    hand->erase(hand->begin() + position);
}
// Check if a player won
bool winner(vector<pair <int, int> > * hand)
{
    if(hand->size() == 0)
    {
        return true;
    }
    return false;
}
//Convert vector data in a long string
void vectorToString(vector<pair <int, int> > * hand,char  *buffer)
{
    string hand_s;
    int j = 0;
    for(int i = 0; i < hand->size(); i++)
    {
        hand_s+= to_string(hand->at(i).first);
        hand_s+= ":";
        hand_s+= to_string(hand->at(i).second);
        if(i != hand->size() -1)
        {
            hand_s+= ":";
        }

    }
    strcpy(buffer, hand_s.c_str());
}
