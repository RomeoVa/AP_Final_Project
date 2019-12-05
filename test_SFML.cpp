#include <SFML/Graphics.hpp>
#include <iostream>
#include "uno_functions.h"

using namespace std;

#define MAX 7

void renderWindow(std::vector<sf::RectangleShape> * rectangles, std::vector<sf::Text> * texts,vector<pair <int, int> > * hand,sf::Text * current_text,sf::RectangleShape * current);
void createVisualCards(vector<pair <int, int> > * hand, std::vector<sf::RectangleShape> * rectangles,std::vector<sf::Text> * texts,sf::Text current_text,sf::RectangleShape current);
int main()
{
    vector<pair <int, int> >  hand;

    std::vector<sf::RectangleShape> rectangles;
    std::vector<sf::Text> texts;
    sf::RectangleShape  *current;
    //sf::Text texts[MAX];
    sf::Text *current_text;

  


    hand = player_hand();

    //createVisualCards(&hand);

    for (int i = 0; i<hand.size(); i++) 
        cout << hand[i].first << ":"<< getColor(&hand[i])<<"   "; 

    //rectangles = createVisualCards(&hand);
    createVisualCards(&hand,&rectangles,&texts,current_text,current);
    renderWindow( &rectangles,&texts, &hand,current_text,current);


    return 0;
}

void renderWindow(  std::vector<sf::RectangleShape> * rectangles, std::vector<sf::Text> * texts,vector<pair <int, int> > * hand,sf::Text * current_text,sf::RectangleShape * current)
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

        for(int i = 0;i < hand->size();i++)
        {
            //cout<<"ENTRA AL ARREGLO"<<endl;
            //window.draw(texts[i]);
            window.draw(rectangles->at(i));
        }

        window.display();
    }
}

void createVisualCards(vector<pair <int, int> > * hand, std::vector<sf::RectangleShape> * rectangles,std::vector<sf::Text> * texts,sf::Text current_text,sf::RectangleShape current)
{
    sf::RectangleShape rectangle;
    sf::Text text;
    sf::Font font;
    font.loadFromFile("arial.ttf");
    current_text.setPosition(sf::Vector2f(500,300));
    current_text.setFont(font);
    current_text.setCharacterSize(20);
    current_text.setString("jhff");
    current.setPosition(sf::Vector2f(500,300));
    current.setSize(sf::Vector2f(50,100));
    current.setFillColor(sf::Color(128,128,128,128));
    current.setOutlineColor(sf::Color(255,255,255));
    current.setOutlineThickness(2.0);
    int x = 10;
    int y = 10;
    int x_text = 25;
    int y_text = 100;
    //cout<<"tamano"<<(*rectangles)[0]<<endl;
    for(int i = 0;i < hand->size();i++)
    {
        rectangles->push_back(rectangle);
        texts->push_back(text);
        cout<<"ENTRA AL FOR "<<i<<endl;
        (*rectangles)[i].setPosition(sf::Vector2f(x,10));
        (*rectangles)[i].setSize(sf::Vector2f(50,100));
        (*rectangles)[i].setFillColor(sf::Color(128,128,128,128));
        (*rectangles)[i].setOutlineColor(sf::Color(255,255,255));
        (*rectangles)[i].setOutlineThickness(2.0);

        (*texts)[i].setPosition(sf::Vector2f(x_text,50));
        (*texts)[i].setFont(font);
        (*texts)[i].setCharacterSize(20);
        (*texts)[i].setString("6");
        x+=50;
        x_text+= 50;
        //cout<<"Sale del FOR"<<i<<endl;
    }
    
}
