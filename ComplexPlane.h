#pragma once
#include <SFML/Graphics.hpp>

class ComplexPlane : public sf::Drawable
{
public:
    ComplexPlane(int pixelWidth, int pixelHeight);

    void updateRender();
    void zoomIn();
    void zoomOut();
    void setCenter(sf::Vector2i mousePixel);
    void setMouseLocation(sf::Vector2i mousePixel);
    void loadText(sf::Text& text);

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Vector2f mapPixelToCoords(int px, int py) const;
    size_t countIterations(const sf::Vector2f& coord) const;
    void iterationsToRGB(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b) const;

    enum class State { CALCULATING, DISPLAYING };
    State m_State;

    sf::VertexArray m_vArray;
    int m_pixelWidth, m_pixelHeight;
    float m_aspectRatio;
    sf::Vector2f m_plane_center;
    sf::Vector2f m_plane_size;
    int m_zoomCount;
    sf::Vector2f m_mouseLocation;

    static constexpr float BASE_WIDTH = 4.0f;
    static constexpr float BASE_HEIGHT = 4.0f;
    static constexpr float BASE_ZOOM = 0.5f;
    static constexpr size_t MAX_ITER = 300;
};
