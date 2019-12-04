#ifndef UNO_FUNCTIONS_H
#define UNO_FUNCTIONS_H

#include <stdio.h>  
#include <stdlib.h>    
#include <time.h>     
#include <iostream>
#include <vector> 

using namespace std;

bool verifyCard(pair <int,int> * card,pair <int,int> * deck_card);
bool wild_card();
bool winner(vector<pair <int, int> > * hand);
pair <int, int> get_card();
string getColor(pair <int,int> * card);
vector<pair <int, int> > player_hand();
void deleteCardAtPosition(vector<pair <int, int> > * hand,int position);

#endif  