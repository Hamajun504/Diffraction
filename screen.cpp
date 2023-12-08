#include <vector>
#include <complex>
#include <utility>
#include <exception>
#include <cmath>
#include <iostream>
#include "source.cpp"
#pragma once
using std::size_t, std::complex;

class Screen final
{
    std::vector<std::vector<double>> field_m;
    std::pair<size_t, size_t> shape_m;
    float scale_m;

    void evaluate_intensity_in_point(size_t, size_t, int, int, const Source&);

public:
    Screen(size_t, size_t, float);

    const std::pair<size_t, size_t>& size() const;

    bool evaluate_intensity(const Source&);

    std::vector<std::vector<double>> normalized_intensity() const;

    std::vector<std::vector<double>> normalized_log_intensity() const;

    std::vector<std::vector<double>> normalized_log_sqr_intensity() const;

    // std::vector<std::vector<double>> normalized_exp_intensity() const;

};

std::ostream& operator<< (std::ostream&, const std::vector<std::vector<double>>&);



Screen::Screen(size_t Nx, size_t Ny, float scale): 
    field_m(Nx, std::vector<double>(Ny, 0)), shape_m(Nx, Ny), scale_m(scale) {}

void Screen::evaluate_intensity_in_point(size_t arr_y, size_t arr_z, int y, int z, const Source& s)
{
    double ny, nz;
    ny = y * 1.0 / shape_m.first * s.wavelength() / s.size().first * scale_m;
    nz = z * 1.0 / shape_m.second * s.wavelength() / s.size().first * scale_m;
    double len = sqrt(1 + ny * ny + nz * nz);
    ny /= len;
    nz /= len;

    complex<double> field(0);
    for (size_t i = 0; i < s.shape().first; ++i)
        for (size_t j = 0; j < s.shape().second; ++j)
            if (s[i][j]) {
                double dist = ny * i / s.shape().first * s.size().first +
                    nz * j / s.shape().second * s.size().second;
                
                field += std::exp(complex<double>(0, 1) * complex<double>(2 * M_PI / s.wavelength() * dist, 0));
            }
    field_m[arr_y][arr_z] = norm(field);
}

const std::pair<size_t, size_t>& Screen::size() const
{
    return shape_m;
}


bool Screen::evaluate_intensity(const Source& source)
{
    for (size_t i = 0; i < shape_m.first; ++i)
        for (size_t j = 0; j < shape_m.second; ++j) {
            evaluate_intensity_in_point(i, j,
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
            if (max < field_m[i][j]) 
                max = field_m[i][j];
        }

    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[0].size(); ++j) {
            out[i][j] = field_m[i][j] * 255 / max;
        }
    return out;
}

std::vector<std::vector<double>> Screen::normalized_log_intensity() const
{
    std::vector<std::vector<double>> out;
    out.resize(field_m.size());
    for (auto& i : out)
        i.resize(field_m[0].size());

    double max = 0;
    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[0].size(); ++j) {
            double l = log(1 + field_m[i][j]);
            if (max < l) 
                max = l;
        }

    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[0].size(); ++j) {
            out[i][j] = log(1 + field_m[i][j]) / max * 255;
        }
    return out;
}

std::vector<std::vector<double>> Screen::normalized_log_sqr_intensity() const
{
    std::vector<std::vector<double>> out;
    out.resize(field_m.size());
    for (auto& i : out)
        i.resize(field_m[0].size());

    double max = 0;
    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[0].size(); ++j) {
            double l = log(1 + field_m[i][j]) * log(1 + field_m[i][j]);
            if (max < l) 
                max = l;
        }

    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[0].size(); ++j) {
            out[i][j] = log(1 + field_m[i][j]) * log(1 + field_m[i][j]) / max * 255;
        }
    return out;
}

std::ostream& operator<< (std::ostream& os, const std::vector<std::vector<double>>& img)
{
    for (size_t i = 0; i < img.size(); ++i)
        for (size_t j = 0; j < img[i].size(); ++j) {
            os << img[i][j];
            if (j + 1 == img[0].size())
                os << std::endl;
            else
                os << ',';
        }

    return os;
}


