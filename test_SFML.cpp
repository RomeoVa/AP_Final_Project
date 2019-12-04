#include <SFML/Graphics.hpp>
#include <iostream>
#include "uno_functions.h"

using namespace std;

#define MAX 7

void renderWindow( sf::RectangleShape * rectangles,sf::Text * texts);
sf::RectangleShape * createVisualCards(vector<pair <int, int> > * hand);

int main()
{
    int x = 10;
    int y = 10;
    int x_text = 25;
    int y_text = 100;
    vector<pair <int, int> >  hand;

   
    sf::Font font;
    font.loadFromFile("arial.ttf");

    sf::RectangleShape * rectangles;

    sf::RectangleShape current;
    sf::Text texts[MAX];
    sf::Text current_text;

    current_text.setPosition(sf::Vector2f(500,300));
    current_text.setFont(font);
    current_text.setCharacterSize(20);
    current_text.setString("jhff");


    current.setPosition(sf::Vector2f(500,300));
    current.setSize(sf::Vector2f(50,100));
    current.setFillColor(sf::Color(128,128,128,128));
    current.setOutlineColor(sf::Color(255,255,255));
    current.setOutlineThickness(2.0);

/*
    for(int i = 0;i < 6;i++)
    {
        rectangles[i].setPosition(sf::Vector2f(x,10));
        rectangles[i].setSize(sf::Vector2f(50,100));
        rectangles[i].setFillColor(sf::Color(128,128,128,128));
        rectangles[i].setOutlineColor(sf::Color(255,255,255));
        rectangles[i].setOutlineThickness(2.0);
        

        texts[i].setPosition(sf::Vector2f(x_text,50));
        texts[i].setFont(font);
        texts[i].setCharacterSize(20);
        texts[i].setString("6");
        x+=50;
        x_text+= 50;

    }
    */

    hand = player_hand();

    //createVisualCards(&hand);

    for (int i = 0; i<hand.size(); i++) 
        cout << hand[i].first << ":"<< getColor(&hand[i])<<"   "; 

    //rectangles = createVisualCards(&hand);
    rectangles = createVisualCards(&hand);
    renderWindow( rectangles,texts);


    return 0;
}

void renderWindow( sf::RectangleShape * rectangles,sf::Text * texts)
{
    sf::RenderWindow window(sf::VideoMode(1000, 600), "UNO++");
    cout<<"ENTRA A LA FUNCION"<<endl;
    
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

        for(int i = 0;i < MAX;i++)
        {
            //cout<<"ENTRA AL ARREGLO"<<endl;
            window.draw(texts[i]);
            window.draw(rectangles[i]);
        }

        window.display();
    }
}

sf::RectangleShape * createVisualCards(vector<pair <int, int> > * hand)
{
    sf::RectangleShape  rectangles[MAX];
    //rectangles = (sf::RectangleShape *) malloc (7 * sizeof (sf::RectangleShape));
    //cout<<"ENTRA A LA sdfasdfs"<<rectangles->m_size() <<endl;
    if (!rectangles)
    {
        printf("Cannot allocate memory\n");
    }
    int x = 10;
    int y = 10;
    int x_text = 25;
    int y_text = 100;

    for(int i = 0;i < MAX;i++)
    {
         cout<<"ENTRA AL FOR"<<i<<endl;
        rectangles[i].setPosition(sf::Vector2f(x,10));
        rectangles[i].setSize(sf::Vector2f(50,100));
        rectangles[i].setFillColor(sf::Color(128,128,128,128));
        rectangles[i].setOutlineColor(sf::Color(255,255,255));
        rectangles[i].setOutlineThickness(2.0);
        x+=50;
    }
    return rectangles;
    
}
