#include <vector>
#include <complex>
#include <utility>
#include <exception>
#include <cmath>
#include "source.cpp"
#pragma once
using std::size_t, std::complex;

#include <iostream>

class Screen final
{
    std::vector<std::vector<complex<double>>> field_m;
    std::pair<size_t, size_t> shape_m;
    float scale_m;

    void evaluate_phase_in_point(size_t, size_t, int, int, const Source&);

public:
    Screen(size_t, size_t, float);

    const std::pair<size_t, size_t>& size() const;

    bool evaluate_phase(const Source&);

    std::vector<std::vector<double>> normalized_intensity() const;
    
};

Screen::Screen(size_t Nx, size_t Ny, float scale): 
    field_m(Nx, std::vector<complex<double>>(Ny, 0)), shape_m(Nx, Ny), scale_m(scale) {}

void Screen::evaluate_phase_in_point(size_t arr_y, size_t arr_z, int y, int z, const Source& s)
{
    double ny, nz;
    ny = y * 1.0 / shape_m.first * s.wavelength() / s.size().first * scale_m;
    nz = z * 1.0 / shape_m.second * s.wavelength() / s.size().first * scale_m;
    double len = sqrt(1 + ny * ny + nz * nz);
    ny /= len;
    nz /= len;

    for (size_t i = 0; i < s.shape().first; ++i)
        for (size_t j = 0; j < s.shape().second; ++j)
            if (s[i][j]) {
                double dist = ny * i / s.shape().first * s.size().first +
                    nz * j / s.shape().second * s.size().second;
                
                field_m[arr_y][arr_z] += std::exp(complex<double>(0, 1) * complex<double>(2 * M_PI / s.wavelength() * dist, 0));
            }

}

const std::pair<size_t, size_t>& Screen::size() const
{
    return shape_m;
}


bool Screen::evaluate_phase(const Source& source)
{
    for (size_t i = 0; i < shape_m.first; ++i)
        for (size_t j = 0; j < shape_m.second; ++j) {
            evaluate_phase_in_point(i, j,
                static_cast<int>(i) - static_cast<int>(shape_m.first) / 2, 
                static_cast<int>(j) - static_cast<int>(shape_m.second) / 2, source);
        }
    return true;
}

std::vector<std::vector<double>> Screen::normalized_intensity() const
{
    std::vector<std::vector<double>> out;
    out.resize(field_m.size());
    for (auto& i : out)
        i.resize(field_m[0].size());

    double max = 0;
    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[0].size(); ++j) {
            out[i][j] = norm(field_m[i][j]);
            if (max < out[i][j]) 
                max = out[i][j];
        }

    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[0].size(); ++j) {
            out[i][j] *= 255 / max;
        }
    return out;
}