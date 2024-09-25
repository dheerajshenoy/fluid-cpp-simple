#pragma once
#include <iostream>
#include <cmath>
#include <cstdint>
#include <raylib.h>
#include <raymath.h>
#include "Rectangle.hpp"
#include <vector>

class FluidCube {

public:
    FluidCube(const int& diffusion, const int& viscosity, const float& dt);
    ~FluidCube();

    void addRectangle(dj::Rectangle& rect, const float& density = 10.0) noexcept;
    void render() noexcept;
    void addDensity(const int& x, const int& y, const float& amount) noexcept;
    void addVelocity(const int& x, const int& y, const float& amountX, const float& amountY) noexcept;
    void simulate() noexcept;
    void set_bnd(const int& b, float *x) noexcept;
    void lin_solve(const int& b, float *x, const float *x0, const float& a, const float& c) noexcept;
    void advect(const int& b, float *d, float *d0,  float *velocX, float *velocY, const float& dt) noexcept;
    void project(float *velocX, float *velocY, float *p, float *div) noexcept;
    void diffuse (const int& b, float *x, float *x0, const float& diff, const float& dt) noexcept;

private:
    int m_iter = 4;

    float m_dt;
    float m_diff;
    float m_visc;

    float *m_s = nullptr;
    float *m_density = nullptr;

    float *m_Vx = nullptr;
    float *m_Vy = nullptr;

    float *m_Vx0 = nullptr;
    float *m_Vy0 = nullptr;

    std::vector<dj::Rectangle> m_rect_list;
};
