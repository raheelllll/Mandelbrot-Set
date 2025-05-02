#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"

int main()
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set (Multithreaded, Colored)");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf"); // Make sure you have a font file

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);

    ComplexPlane mandelbrot(WIDTH, HEIGHT);
    mandelbrot.updateRender();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    mandelbrot.setCenter({event.mouseButton.x, event.mouseButton.y});
                    mandelbrot.zoomIn();
                    mandelbrot.updateRender();
                }
                else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    mandelbrot.setCenter({event.mouseButton.x, event.mouseButton.y});
                    mandelbrot.zoomOut();
                    mandelbrot.updateRender();
                }
            }
            if (event.type == sf::Event::MouseMoved)
            {
                mandelbrot.setMouseLocation({event.mouseMove.x, event.mouseMove.y});
            }
        }

        mandelbrot.loadText(text);

        window.clear();
        window.draw(mandelbrot);
        window.draw(text);
        window.display();
    }
    return 0;
}
