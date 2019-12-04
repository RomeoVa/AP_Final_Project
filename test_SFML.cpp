#include <SFML/Graphics.hpp>

#define MAX 7

int main()
{
    int x = 10;
    int y = 10;
    int x_text = 25;
    int y_text = 100;
    sf::RenderWindow window(sf::VideoMode(1000, 600), "UNO++");
    sf::Font font;
    font.loadFromFile("arial.ttf");

    sf::RectangleShape rectangles[MAX];
    sf::RectangleShape current;
    sf::Text texts[MAX];


    current.setPosition(sf::Vector2f(500,300));
    current.setSize(sf::Vector2f(50,100));
    current.setFillColor(sf::Color(128,128,128,128));
    current.setOutlineColor(sf::Color(255,255,255));
    current.setOutlineThickness(2.0);

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
            window.draw(texts[i]);
            window.draw(rectangles[i]);
        }
        window.draw(current);
        rectangles[6].setPosition(sf::Vector2f(400,10));
        rectangles[6].setSize(sf::Vector2f(50,100));
        //rectangles[6].setFillColor(sf::Color::Red);
        rectangles[6].setFillColor(sf::Color(255,0,0,100));
        rectangles[6].setOutlineColor(sf::Color(255,255,255));
        rectangles[6].setOutlineThickness(2.0);
        

        texts[6].setPosition(sf::Vector2f(410,50));
        texts[6].setFont(font);
        texts[6].setFillColor(sf::Color::White);
        texts[6].setCharacterSize(30);
        texts[6].setString("6");

        window.display();
    }
}