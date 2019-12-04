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


pair <int, int> get_card()
{
    pair <int, int> card ; 
    int number;
    int color;
    
    if (wild_card())
    {
        // cout << "WILD CARD ";
        number = -1;
        color = 0;
    }
    else
    {
        // cout << "NORMAL CARD ";
        number = rand() % 13 ;  // random number between 0-12
        color = rand() % 4 + 1 ;  // random number between 1-4
    }
    card.first = number; 
    card.second = color; 

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

vector<pair <int, int> > player_hand()
{
    vector<pair <int, int> > hand;

    for (int i = 0; i < 7; i++) 
    {
        hand.push_back(get_card()); 
    }

    return hand;
       
}

bool verifyCard(pair <int,int> * card,pair <int,int> * deck_card)
{
    if(card->first == deck_card->first || card->second == deck_card->second)
    {
        return true;
    }
    else if (card->first == -1 || card->second == 0) // WILD CARD
    {
        return true;
    }
    return false;
}

string getColor(pair <int,int> * card)
{
    switch (card->second)
    {
        case BLACK: return "Black";
        case RED: return "Red";
        case BLUE: return "Blue";
        case GREEN: return "Green";
        case YELLOW: return "Yellow";
    }
    
}

void deleteCardAtPosition(vector<pair <int, int> > * hand,int position)
{
    hand->erase(hand->begin() + position);
}

bool winner(vector<pair <int, int>> * hand)
{
    if(hand->size() == 0)
    {
        return true;
    }
    return false;
}

void vectorToString(vector<pair <int, int>> * hand,char  *buffer)
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

    cout<<"JHASKJdahk"<<buffer<<endl;
}
/*
int main()
{

    vector<pair <int, int> >  hand;
    string name;
    srand (time(NULL));
    char buffer[BUFFER_SIZE];

    
    map<int, pair <string ,int> > player; 
    player.insert(make_pair(1,make_pair("Mau",6)));
    //cout<<"player: "<<player[1].first<<player[1].second<<endl;

    cout<< "WELCOME TO UNO++"<<endl;
    cout<< "Please enter your name:";
    cin>> name;


    
    hand = player_hand();
    vectorToString(&hand, buffer);
    cout<<"AGAP"<<buffer<<endl;
    
    for (int i = 0; i<hand.size(); i++) 
        cout << hand[i].first << ":"<< getColor(&hand[i])<<"   "; 

    

    cout<<endl;


    for (int i = 0; i<hand.size(); i++) 
        cout <<"   "<< i << "      ";   
    
    //cout<<"begin: "<<hand.begin()<<endl;

    cout<<endl;

    deleteCardAtPosition(&hand,4);
    deleteCardAtPosition(&hand,2);

    //hand.erase(hand.begin() + 1);

     for (int i = 0; i<hand.size(); i++) 
        cout << hand[i].first << ":"<< getColor(&hand[i])<<"   "; 


     cout<<endl;

    for (int i = 0; i<hand.size(); i++) 
        cout <<"   "<< i << "      ";   
    
        
        
    cout<<endl;



    return 0;
}
*/