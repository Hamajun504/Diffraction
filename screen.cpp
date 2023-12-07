#include <vector>
#include <complex>
#include <utility>
#include <exception>
#include <cmath>
#include <source.cpp>
#pragma once
using std::size_t, std::complex;

class Screen final
{
    std::vector<std::vector<complex<double>>> field_m;
    std::pair<size_t, size_t> shape_m;
    float scale_m;

    void evaluate_phase_in_point(size_t, size_t, const Source&);

public:
    Screen(size_t, size_t, float);

    const std::pair<size_t, size_t>& size() const;

    bool evaluate_phase(const Source&);
    
};

Screen::Screen(size_t Nx, size_t Ny, float scale): 
    field_m(Nx, std::vector<complex<double>>(Ny, 0)), shape_m(Nx, Ny), scale_m(scale) {}

void Screen::evaluate_phase_in_point(size_t x, size_t y, const Source& s)
{
    double sin_phi_z_sqr = scale_m * scale_m * x * y / (shape_m.first * shape_m.first);
    double sin_phi_y_sqr = scale_m * scale_m * x * y / (shape_m.second * shape_m.second);
    double cos_phi_y_sqr = 1 - sin_phi_y_sqr;
    for (size_t i = 0; i < s.shape().first; ++i)
        for (size_t j; j < s.shape().second; ++j)
            if (s[i][j])
            {
                double dist = cos_phi_y_sqr * sin_phi_z_sqr * (s.size().first * i) * (s.size().first * i) / (s.shape().first * s.shape().first) +
                    sin_phi_y_sqr * (s.size().second * j) * (s.size().second * j) / (s.shape().second * s.shape().second);
                field_m[i][j] += std::exp(complex<double>(0, 1) * complex<double>(2 * M_PI / s.wavelength() * dist, 0));
            }
}

const std::pair<size_t, size_t>& Screen::size() const
{
    return shape_m;
}


bool Screen::evaluate_phase(const Source& source)
{
    for (size_t i = 0; i < shape_m.first; ++i)
        for (size_t j = 0; j < shape_m.second; ++j)
            evaluate_phase_in_point(i, j, source);
}