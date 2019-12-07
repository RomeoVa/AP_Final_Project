#include <SFML/Graphics.hpp>
#include <iostream>
#include "uno_functions.h"
#include "graphics.h"

using namespace std;

#define MAX 7
#define BUFFER_SIZE 1024


/*
int main()
{
    vector<pair <int, int> >  hand;
    char cards[BUFFER_SIZE];
    std::vector<sf::RectangleShape> rectangles;
    std::vector<sf::Text> texts;
    std::vector<pair<string,int>> players;
    sf::RectangleShape  *current;
    sf::Text *current_text;
    pair <int,int> current_card;

    current_card.first = 4;
    current_card.second = 2;
    players.push_back(make_pair("Mau",7));
    players.push_back(make_pair("Romeo",5));
    players.push_back(make_pair("Pablo",3));
    players.push_back(make_pair("Gil",5));

    hand = player_hand();

    for (int i = 0; i<hand.size(); i++) 
        cout << hand[i].first << ":"<< getColor(&hand[i])<<"   "; 

    renderWindow(&hand,&current_card, &players,3);


    return 0;
}
*/

void renderWindow(vector<pair <int, int> > *  hand, pair <int,int> * current_card,vector<pair <string, int> > *  players,int turn)
{
    std::vector<sf::RectangleShape> rectangles;
    sf::RectangleShape current_rectangle;
    std::vector<sf::Text> texts;
    std::vector<sf::Text> decisions_texts;
    sf::Text current_text, turn_text;

    sf::RenderWindow window(sf::VideoMode(1000, 600), "UNO++");
    
    std::vector<sf::Text> names_texts;
    sf::Font font;
    string number, current_number,name,turn_string;
    int x, y,x_turn,y_turn;
    int x_text = 25;

    window.setPosition(sf::Vector2i(0,20));
    names_texts.resize(players->size());
    font.loadFromFile("arial.ttf");
    rectangles = createRectangles(hand);
    current_rectangle = createCurrentRectangle(current_card);
    texts.resize(hand->size());
    decisions_texts.resize(hand->size());


    for(int i = 0;i < hand->size();i++)
    {
        number = to_string(hand->at(i).first);
        //cout<<"ENTRA AL FOR del text"<<i<<endl;
        texts[i].setPosition(sf::Vector2f(x_text,50));
        texts[i].setFont(font);
        texts[i].setCharacterSize(20);
        texts[i].setString(getSpecialCard(number));

        decisions_texts[i].setPosition(sf::Vector2f(x_text,120));
        decisions_texts[i].setFont(font);
        decisions_texts[i].setCharacterSize(20);
        decisions_texts[i].setString(getSpecialCard(to_string(i+1)));
        x_text+= 50;
    }
    
    for(int i = 0;i < players->size();i++)
    {
        x = getCoords(i).first;
        y = getCoords(i).second;
        name = players->at(i).first;
        name+=":\n";
        name+= to_string( players->at(i).second);
        //cout<<"ENTRA AL FOR del nombre"<<i<<endl;
        names_texts[i].setPosition(sf::Vector2f(x,y));
        names_texts[i].setFont(font);
        names_texts[i].setCharacterSize(20);
        names_texts[i].setString(name);
        cout<<"NAME"<< name<<endl;
    }

    current_number = to_string(current_card->first);
    current_text.setPosition(sf::Vector2f(500,300));
    current_text.setFont(font);
    current_text.setCharacterSize(20);
    current_text.setString(current_number);

    x_turn = getTurnCoords(turn).first;
    y_turn = getTurnCoords(turn).second;
    turn_text.setPosition(sf::Vector2f(x_turn,y_turn));
    turn_text.setFont(font);
    turn_text.setCharacterSize(20);
    turn_text.setString(getArrowTurn(turn));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Color darkGreyGreen(10, 30, 10, 255);
        window.clear(darkGreyGreen);

        window.draw(current_rectangle);
        window.draw(current_text);
        window.draw(turn_text);

        for(int i = 0;i < hand->size();i++)
        {
            window.draw(rectangles[i]);
            window.draw(texts[i]);
            window.draw(decisions_texts[i]);
        }

        for(int i = 0;i < players->size();i++)
        {           
            window.draw(names_texts[i]);
        }
        window.display();
    }
}

std::vector<sf::RectangleShape> createRectangles(vector<pair <int, int> > *  hand)
{
    std::vector<sf::RectangleShape> rectangles;
    sf::RectangleShape rectangle;
     int x = 10;
    rectangles.resize(hand->size());
    for(int i = 0;i < hand->size();i++)
    {
        rectangles[i].setPosition(sf::Vector2f(x,10));
        rectangles[i].setSize(sf::Vector2f(50,100));
        rectangles[i].setFillColor(getRectangleColor(hand->at(i).second));
        rectangles[i].setOutlineColor(sf::Color(255,255,255));
        rectangles[i].setOutlineThickness(2.0);
        x+=50;
    }
    return rectangles; 
}

sf::RectangleShape createCurrentRectangle(pair <int,int> * current_card)
{
    sf::RectangleShape current_rectangle;
    //cout<<"ENTRA AL CURRENT"<<endl;
    current_rectangle.setPosition(sf::Vector2f(480,260));
    current_rectangle.setSize(sf::Vector2f(50,100));
    current_rectangle.setFillColor(getRectangleColor(current_card->second));
    current_rectangle.setOutlineColor(sf::Color(255,255,255));
    current_rectangle.setOutlineThickness(2.0);

    return current_rectangle;
}

sf::Color getRectangleColor(int id_color)
{
    sf::Color color;
    if(id_color == BLACK)
    {
       color = sf::Color(255,255,255,120);
    }
    else if(id_color == RED)
    {
        color = sf::Color(255,0,0,120);   
    }
    else if(id_color == BLUE)
    {
        color = sf::Color(0,0,255,120);   
    }
    else if(id_color == GREEN)
    {
        color = sf::Color(0,255,0,120);   
    }
    else if(id_color == YELLOW)
    {
        color = sf::Color(255,255,0,120);   
    }
    return color;
}

pair<int,int> getCoords(int player)
{
    pair<int,int> coords;
    if(player == 0)
    {
        coords.first = 500;
        coords.second = 150;
    }
    else if(player == 1)
    {
        coords.first = 900;
        coords.second = 300;
    }
    else if(player == 2)
    {
        coords.first = 500;
        coords.second = 550;
    }
    else if(player == 3)
    {
        coords.first = 10;
        coords.second = 300;
    }
    return coords;
}

pair<int,int> getTurnCoords(int turn)
{
    pair<int,int> coords;
    if(turn == 0)
    {
        coords.first = 500-20;
        coords.second = 150;
    }
    else if(turn == 1)
    {
        coords.first = 900-20;
        coords.second = 300;
    }
    else if(turn == 2)
    {
        coords.first = 500-20;
        coords.second = 550;
    }
    else if(turn == 3)
    {
        //coords.first = textSize;
        coords.first = 50;
        coords.second = 300;
    }
    return coords;
}

string getArrowTurn(int turn)
{
    string arrow = "->";
    if(turn == 3)
    {
        arrow = "<-";
    }
    return arrow;
}

string getSpecialCard(string number)
{
    if(number == "12")
    {
        number = "+2";
    }
    return number;
}