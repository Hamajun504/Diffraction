#include <vector>
#include <complex>
#include <utility>
#include <exception>
#include <cmath>
#include <iostream>
#include <thread>
#include "source.cpp"
#pragma once
using std::complex, std::size_t;

class Screen final
{
    std::vector<std::vector<double>> field_m;
    std::pair<size_t, size_t> shape_m;
    size_t max_threads_m;
    float scale_m;
    const Source& source_m;

    void evaluate_intensity_in_point(size_t, size_t, int, int);

    void evaluate_intensity_in_row(size_t);

public:
    Screen(size_t, size_t, float, const Source&, size_t);

    const std::pair<size_t, size_t>& size() const;

    void evaluate_intensity();

    std::vector<std::vector<double>> normalized_intensity() const;

    std::vector<std::vector<double>> normalized_log_intensity() const;

    std::vector<std::vector<double>> normalized_log_sqr_intensity() const;

    // std::vector<std::vector<double>> normalized_exp_intensity() const;

};

std::ostream& operator<< (std::ostream&, const std::vector<std::vector<double>>&);



Screen::Screen(size_t Nx, size_t Ny, float scale, const Source& source, size_t max_threads=4): 
    field_m(Nx, std::vector<double>(Ny, 0)), shape_m(Nx, Ny), scale_m(scale), source_m(source), max_threads_m(max_threads) {}

void Screen::evaluate_intensity_in_point(size_t arr_y, size_t arr_z, int y, int z)
{
    double ny, nz;
    ny = y * 1.0 / shape_m.first * source_m.wavelength() / source_m.size().first * scale_m;
    nz = z * 1.0 / shape_m.second * source_m.wavelength() / source_m.size().first * scale_m;
    double len = sqrt(1 + ny * ny + nz * nz);
    ny /= len;
    nz /= len;

    complex<double> field(0);
    for (size_t i = 0; i < source_m.shape().first; ++i)
        for (size_t j = 0; j < source_m.shape().second; ++j)
            if (source_m[i][j]) {
                double dist = ny * i / source_m.shape().first * source_m.size().first +
                    nz * j / source_m.shape().second * source_m.size().second;
                
                field += std::exp(complex<double>(0, 1) * complex<double>(2 * M_PI / source_m.wavelength() * dist, 0));
            }
    field_m[arr_y][arr_z] = norm(field);
}

void Screen::evaluate_intensity_in_row(size_t i)
{
    for (size_t j = 0; j < shape_m.second; ++j) {
            evaluate_intensity_in_point(i, j,
                static_cast<int>(i) - static_cast<int>(shape_m.first) / 2, 
                static_cast<int>(j) - static_cast<int>(shape_m.second) / 2);
        }
}

const std::pair<size_t, size_t>& Screen::size() const
{
    return shape_m;
}


void Screen::evaluate_intensity()
{
    std::thread threads[max_threads_m];
    for (size_t i = 0; i < shape_m.first; i += max_threads_m) {
        for (size_t th = 0; th < max_threads_m && i + th < shape_m.first; ++th)
            threads[th] = std::thread(&Screen::evaluate_intensity_in_row, this, i + th);
        for (auto& th : threads) th.join(); 
    }  
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

    auto normalize = [this, &out, &max](size_t i)
    {
        for (size_t j = 0; j < out[i].size(); ++j) {
            out[i][j] = field_m[i][j] * 255 / max;
        }
    };

    std::thread threads[max_threads_m];
    for (size_t i = 0; i < out.size(); i += max_threads_m) {
        for (size_t th = 0; th < max_threads_m; ++th)
            threads[th] = std::thread(normalize, i + th);
        for (auto& th : threads) 
            th.join();
    }
    return out;
}

std::vector<std::vector<double>> Screen::normalized_log_intensity() const
{
    std::vector<std::vector<double>> out;
    out.resize(field_m.size());
    for (auto& i : out)
        i.resize(field_m[0].size());

    auto not_normalized = [this, &out](size_t i){
        for (size_t j = 0; j < out[i].size(); ++j) {
            out[i][j] = log(1 + field_m[i][j]);
        }
    };

    std::thread threads[max_threads_m];
    for (size_t i = 0; i < out.size(); i += max_threads_m) {
        for (size_t th = 0; th < max_threads_m; ++th)
            threads[th] = std::thread(not_normalized, i + th);
        for (auto& th : threads) 
            th.join();
    }

    double max = 0;
    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[i].size(); ++j) {
            double l = log(1 + field_m[i][j]);
            if (max < out[i][j]) 
                max = out[i][j];
        }

    auto normalize = [this, &out, &max](size_t i)
    {
        for (size_t j = 0; j < out[i].size(); ++j) {
            out[i][j] = out[i][j] / max * 255;
        }
    };

    for (size_t i = 0; i < out.size(); i += max_threads_m) {
        for (size_t th = 0; th < max_threads_m; ++th)
            threads[th] = std::thread(normalize, i + th);
        for (auto& th : threads) 
            th.join();
    }

    return out;
}

std::vector<std::vector<double>> Screen::normalized_log_sqr_intensity() const
{
    std::vector<std::vector<double>> out;
    out.resize(field_m.size());
    for (auto& i : out)
        i.resize(field_m[0].size());

    auto not_normalized = [this, &out](size_t i){
        for (size_t j = 0; j < out[i].size(); ++j) {
            out[i][j] = log(1 + field_m[i][j]) * log(1 + field_m[i][j]);
        }
    };

    std::thread threads[max_threads_m];
    for (size_t i = 0; i < out.size(); i += max_threads_m) {
        for (size_t th = 0; th < max_threads_m; ++th)
            threads[th] = std::thread(not_normalized, i + th);
        for (auto& th : threads) 
            th.join();
    }

    double max = 0;
    for (size_t i = 0; i < out.size(); ++i)
        for (size_t j = 0; j < out[0].size(); ++j) {
            if (max < out[i][j]) 
                max = out[i][j];
        }

    auto normalize = [this, &out, &max](size_t i)
    {
        for (size_t j = 0; j < out[i].size(); ++j) {
            out[i][j] = out[i][j] / max * 255;
        }
    };

    for (size_t i = 0; i < out.size(); i += max_threads_m) {
        for (size_t th = 0; th < max_threads_m; ++th)
            threads[th] = std::thread(normalize, i + th);
        for (auto& th : threads) 
            th.join();
    }

    return out;
}

std::ostream& operator<< (std::ostream& os, const std::vector<std::vector<double>>& img)
{
    for (size_t i = 0; i < img.size(); ++i)
        for (size_t j = 0; j < img[i].size(); ++j) {
            os << round(img[i][j]);
            if (j + 1 == img[0].size())
                os << std::endl;
            else
                os << ',';
        }

    return os;
}


