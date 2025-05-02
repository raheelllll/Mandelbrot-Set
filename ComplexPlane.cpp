#include "ComplexPlane.h"
#include <complex>
#include <sstream>
#include <cmath>
#include <thread>
#include <vector>
#include <SFML/Graphics.hpp>
#include <algorithm>

sf::Color lerp(const sf::Color& a, const sf::Color& b, float t) {
    return sf::Color(
        static_cast<sf::Uint8>(a.r + (b.r - a.r) * t),
        static_cast<sf::Uint8>(a.g + (b.g - a.g) * t),
        static_cast<sf::Uint8>(a.b + (b.b - a.b) * t)
    );
}

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
: m_pixelWidth(pixelWidth)
, m_pixelHeight(pixelHeight)
, m_plane_center(0.f, 0.f)
, m_zoomCount(0)
, m_State(State::CALCULATING)
, m_vArray(sf::Points, pixelWidth * pixelHeight)
{
    m_aspectRatio = float(pixelHeight) / float(pixelWidth);
    m_plane_size.x = BASE_WIDTH;
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio;
}

void ComplexPlane::draw(sf::RenderTarget& target, sf::RenderStates) const
{
    target.draw(m_vArray);
}

void ComplexPlane::updateRender()
{
    if (m_State != State::CALCULATING)
        return;

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // fallback

    std::vector<std::thread> threads(num_threads);
    int rows_per_thread = m_pixelHeight / num_threads;

    auto worker = [this](int y_start, int y_end) {
        for (int i = y_start; i < y_end; ++i) {
            for (int j = 0; j < m_pixelWidth; ++j) {
                int idx = j + i * m_pixelWidth;
                m_vArray[idx].position = sf::Vector2f(float(j), float(i));
                sf::Vector2f c = mapPixelToCoords(j, i);
                size_t it = countIterations(c);
                sf::Uint8 r, g, b;
                iterationsToRGB(it, r, g, b);
                m_vArray[idx].color = sf::Color(r, g, b);
            }
        }
    };

    for (unsigned int t = 0; t < num_threads; ++t) {
        int y_start = t * rows_per_thread;
        int y_end = (t == num_threads - 1) ? m_pixelHeight : (y_start + rows_per_thread);
        threads[t] = std::thread(worker, y_start, y_end);
    }

    for (auto& th : threads)
        th.join();

    m_State = State::DISPLAYING;
}

void ComplexPlane::zoomIn()
{
    ++m_zoomCount;
    float factor = std::pow(BASE_ZOOM, float(m_zoomCount));
    m_plane_size.x = BASE_WIDTH * factor;
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * factor;
    m_State = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
    --m_zoomCount;
    float factor = std::pow(BASE_ZOOM, float(m_zoomCount));
    m_plane_size.x = BASE_WIDTH * factor;
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * factor;
    m_State = State::CALCULATING;
}

void ComplexPlane::setCenter(sf::Vector2i mousePixel)
{
    m_plane_center = mapPixelToCoords(mousePixel.x, mousePixel.y);
    m_State = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(sf::Vector2i mousePixel)
{
    m_mouseLocation = mapPixelToCoords(mousePixel.x, mousePixel.y);
}

void ComplexPlane::loadText(sf::Text& text)
{
    std::ostringstream ss;
    ss << "Mandelbrot Set\n";
    ss << "Center: (" << m_plane_center.x << "," << m_plane_center.y << ")\n";
    ss << "Cursor: (" << m_mouseLocation.x << "," << m_mouseLocation.y << ")\n";
    ss << "Left-click to Zoom in\n";
    ss << "Right-click to Zoom out\n";
    text.setString(ss.str());
}

sf::Vector2f ComplexPlane::mapPixelToCoords(int px, int py) const
{
    float left   = m_plane_center.x - m_plane_size.x * 0.5f;
    float top    = m_plane_center.y - m_plane_size.y * 0.5f;
    float real   = left + (float(px) / float(m_pixelWidth)) * m_plane_size.x;
    float imag   = top  + (float(py) / float(m_pixelHeight)) * m_plane_size.y;
    return sf::Vector2f(real, imag);
}

size_t ComplexPlane::countIterations(const sf::Vector2f& coord) const
{
    std::complex<double> z(0.0, 0.0);
    std::complex<double> c(coord.x, coord.y);
    size_t i = 0;
    while (std::abs(z) < 2.0 && i < MAX_ITER)
    {
        z = z * z + c;
        ++i;
    }
    return i;
}

void ComplexPlane::iterationsToRGB(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b) const
{
    if (count == MAX_ITER) {
        r = g = b = 0;
        return;
    }

    const int regions = 5;
    float norm = static_cast<float>(count) / MAX_ITER;
    const float breaks[6] = {0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    const sf::Color palette[6] = {
        sf::Color(200, 0, 255),
        sf::Color(0, 0, 255),
        sf::Color(0, 255, 255),
        sf::Color(0, 255, 0),
        sf::Color(255, 255, 0),
        sf::Color(255, 0, 0)
    };

    for (int i = 0; i < regions; ++i) {
        if (norm >= breaks[i] && norm < breaks[i+1]) {
            float t = (norm - breaks[i]) / (breaks[i+1] - breaks[i]);
            sf::Color c = lerp(palette[i], palette[i+1], t);
            r = c.r;
            g = c.g;
            b = c.b;
            return;
        }
    }
    r = g = b = 0;
}
