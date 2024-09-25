#pragma once

#include <raylib.h>

namespace dj {
    class Rectangle {

    public:
        Rectangle(const Vector2& position, const float& length,
                  const float &breadth, const Color& color = WHITE) :
                      m_position(position), m_length(length), m_breadth(breadth),
                      m_color(color) {}

        inline void render() noexcept {
            DrawRectangle(m_position.x, m_position.y, m_length, m_breadth, m_color);
        }

        inline float breadth() noexcept { return m_breadth; }
        inline float length() noexcept { return m_length; }

        inline Vector2 position() noexcept { return m_position; }

    private:
        Vector2 m_position;
        float m_length, m_breadth;
        Color m_color;

    };
};

