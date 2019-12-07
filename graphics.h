#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "colors.h"
#include "uno_functions.h"

void renderWindow(vector<pair <int, int> > *  hand, pair <int,int> * current_card,vector<pair <string, int> > *  players,int turn,  sf::RenderWindow * window);
std::vector<sf::RectangleShape> createRectangles(vector<pair <int, int> > *  hand);
sf::Color getRectangleColor(int color);
sf::RectangleShape createCurrentRectangle(pair <int,int> *current_card);
pair<int,int> getCoords(int player);
pair<int,int> getTurnCoords(int turn);
string getArrowTurn(int turn);
string getSpecialCard(string number);

#endif  