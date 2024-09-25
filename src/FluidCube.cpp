#include "FluidCube.hpp"

int IX(int x, int y) {
    return x + (y * N);
}

FluidCube::FluidCube(const int& diffusion, const int& viscosity, const float& dt)
: m_dt(dt), m_diff(diffusion), m_visc(viscosity) {
    m_s = new float[N * N]();
    m_density = new float[N * N]();

    m_Vx = new float[N * N]();
    m_Vy = new float[N * N]();

    m_Vx0 = new float[N * N]();
    m_Vy0 = new float[N * N]();
}

void FluidCube::addDensity(const int& x, const int& y, const float& amount) noexcept
{
    this->m_density[IX(x, y)] += amount;
}

void FluidCube::addVelocity(const int& x, const int& y, const float& amountX, const float& amountY) noexcept
{
    int index = IX(x, y);

    this->m_Vx[index] += amountX;
    this->m_Vy[index] += amountY;
}

void FluidCube::simulate() noexcept
{
    float visc     = this->m_visc;
    float diff     = this->m_diff;
    float dt       = this->m_dt;
    float *Vx      = this->m_Vx;
    float *Vy      = this->m_Vy;
    float *Vx0     = this->m_Vx0;
    float *Vy0     = this->m_Vy0;
    float *s       = this->m_s;
    float *density = this->m_density;

    diffuse(1, Vx0, Vx, visc, dt);
    diffuse(2, Vy0, Vy, visc, dt);

    project(Vx0, Vy0, Vx, Vy);

    advect(1, Vx, Vx0, Vx0, Vy0, dt);
    advect(2, Vy, Vy0, Vx0, Vy0, dt);

    project(Vx, Vy, Vx0, Vy0);

    diffuse(0, s, density, diff, dt);
    advect(0, density, s, Vx, Vy, dt);
}

void FluidCube::set_bnd(const int& b, float *x) noexcept
{
    for(uint32_t i = 1; i < N - 1; i++) {
        x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
        x[IX(i, N-1)] = b == 2 ? -x[IX(i, N-2)] : x[IX(i, N-2)];
    }

    for(uint32_t j = 1; j < N - 1; j++) {
        x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
        x[IX(N-1, j)] = b == 1 ? -x[IX(N-2, j)] : x[IX(N-2, j)];
    }


    x[IX(0, 0)] = 0.5f * (x[IX(1, 0)]
        + x[IX(0, 1)]);

    x[IX(0, N-1)] = 0.5f * (x[IX(1, N-1)]
        + x[IX(0, N-2)]);

    x[IX(N-1, 0)] = 0.5f * (x[IX(N-2, 0)]
        + x[IX(N-1, 1)]);

    x[IX(N-1, N-1)] = 0.5f * (x[IX(N-2, N-1)]
        + x[IX(N-1, N-2)]);

    for(uint32_t k=0; k < m_rect_list.size(); k++) {

    }
}

void FluidCube::lin_solve(const int& b, float *x, const float *x0, const float& a, const float& c) noexcept {
    float cRecip = 1.0 / c;
    for (uint32_t k = 0; k < m_iter; k++) {
        for (uint32_t j = 1; j < N - 1; j++) {
            for (uint32_t i = 1; i < N - 1; i++) {
                x[IX(i, j)] =
                    (x0[IX(i, j)]
                    + a*(    x[IX(i+1, j)]
                    +x[IX(i-1, j)]
                    +x[IX(i  , j+1)]
                    +x[IX(i  , j-1)]
                )) * cRecip;
            }
        }
    }
    set_bnd(b, x);
}

void FluidCube::diffuse (const int& b, float *x, float *x0, const float& diff, const float& dt) noexcept
{
    float a = dt * diff * (N - 2) * (N - 2);
    lin_solve(b, x, x0, a, 1 + 6 * a);
}

void FluidCube::project(float *velocX, float *velocY, float *p,
                        float *div) noexcept {
    for (uint32_t j = 1; j < N - 1; j++) {
        for (uint32_t i = 1; i < N - 1; i++) {
            div[IX(i, j)] = -0.5f *
                (velocX[IX(i + 1, j)] - velocX[IX(i - 1, j)] +
                velocY[IX(i, j + 1)] - velocY[IX(i, j - 1)])/
                N;
            p[IX(i, j)] = 0;
        }
    }
    set_bnd(0, div);
    set_bnd(0, p);
    lin_solve(0, p, div, 1, 6);

    for (uint32_t j = 1; j < N - 1; j++) {
        for (uint32_t i = 1; i < N - 1; i++) {
            velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * N;
            velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * N;
        }
    }
    set_bnd(1, velocX);
    set_bnd(2, velocY);
}

void FluidCube::advect(const int& b, float *d, float *d0, float *velocX,
                       float *velocY, const float& dt) noexcept {

    float i0, i1, j0, j1;
    float dtx = dt * (N - 2);
    float dty = dt * (N - 2);
    float s0, s1, t0, t1;
    float tmp1, tmp2, x, y;
    float Nfloat = (N - 2); // This was crashing
    float ifloat, jfloat;

    int i, j;

    for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
        for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
            tmp1 = dtx * velocX[IX(i, j)];
            tmp2 = dty * velocY[IX(i, j)];
            x = ifloat - tmp1;
            y = jfloat - tmp2;

            if (x < 0.5f)
                x = 0.5f;

            if (x > Nfloat + 0.5f)
                x = Nfloat + 0.5f;

            i0 = floorf(x);
            i1 = i0 + 1.0f;

            if (y < 0.5f)
                y = 0.5f;

            if (y > Nfloat + 0.5f)
                y = Nfloat + 0.5f;

            j0 = floorf(y);
            j1 = j0 + 1.0f;

            s1 = x - i0;
            s0 = 1.0f - s1;
            t1 = y - j0;
            t0 = 1.0f - t1;

            int i0i = i0;
            int i1i = i1;
            int j0i = j0;
            int j1i = j1;

            d[IX(i, j)] =
                s0 * (t0 * d0[IX(i0i, j0i)] + t1 * d0[IX(i0i, j1i)]) +
                s1 * (t0 * d0[IX(i1i, j0i)] + t1 * d0[IX(i1i, j1i)]);
        }
    }

    set_bnd(b, d);
}

void FluidCube::render() noexcept {

    for(uint32_t i=0; i < N; i++) {
        for(uint32_t j=0; j < N; j++) {
            float x = i * SCALE;
            float y = j * SCALE;
            int d = this->m_density[IX(i, j)];
            Color color = { 255, 255, 255, (unsigned char) d };
            DrawRectangle(x, y, SCALE, SCALE, color);

            for(uint32_t k=0; k < m_rect_list.size(); k++) {
                auto rect = m_rect_list.at(i);
                rect.render();
            }
        }
    }
}

void FluidCube::addRectangle(dj::Rectangle& rect, const float& density) noexcept {

    /*// If the shape position is outside the screen, do nothing.*/
    if ((float) rect.position().x / SCALE > screenWidth || (float) rect.position().x / SCALE < 0)
        return;

    if ((float) rect.position().y / SCALE > screenHeight || (float) rect.position().y / SCALE < 0)
        return;

    m_rect_list.push_back(rect);

    for(uint32_t i=0; i < rect.length(); i++)
        for(uint32_t j=0; j < rect.breadth(); j++)
            this->addVelocity(i, j, 0, 0);
}

FluidCube::~FluidCube() {}


