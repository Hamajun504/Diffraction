#include <vector>
#include <utility>
#include <iostream>
#include <thread>
#pragma once


class Source final
{
    std::vector<std::vector<bool>> field_m;
    std::pair<size_t, size_t> shape_m;
    std::pair<double, double> size_m;
    double wavelength_m;
    size_t max_threads_m;

    static bool check_polygon(const std::vector<std::pair<int, int>>&);

    static bool check_point_in_polygon(const std::pair<int, int>&, const std::vector<std::pair<int, int>>&);


public:
    Source(size_t Nx, size_t Ny, double, double, size_t);

    const std::pair<size_t, size_t>& shape() const;

    const std::pair<double, double>& size() const;

    const double wavelength() const;

    const std::vector<bool>& operator[](size_t) const;

    bool mark_points(const std::vector<std::pair<size_t, size_t>>&);

};

Source::Source(size_t Nx, size_t Ny, double width, double wavelength, size_t max_threads=4): 
    field_m(Nx, std::vector<bool>(Ny, false)), shape_m(Nx, Ny), size_m(width, Ny / Nx * width), wavelength_m(wavelength), max_threads_m(max_threads) {}

const std::pair<size_t, size_t>& Source::shape() const
{
    return shape_m;
}

const std::pair<double, double>& Source::size() const
{
    return size_m;
}

const double Source::wavelength() const
{
    return wavelength_m;
}

const std::vector<bool>& Source::operator[] (size_t i) const
{
    return field_m[i];
}

bool Source::check_polygon(const std::vector<std::pair<int, int>>& vertices)
{
    if (vertices.size() < 3)
        return false;
    int cross1, cross2;
    cross2 = (vertices[1].first - vertices[0].first) * (vertices[2].second - vertices[1].second) - 
        (vertices[1].second - vertices[0].second) * (vertices[2].first - vertices[1].first);
    for (size_t i = 1; i <= vertices.size(); ++i) {
        std::swap(cross1, cross2);
        cross2 = (vertices[(i+1)%vertices.size()].first - vertices[i%vertices.size()].first) * 
            (vertices[(i+2)%vertices.size()].second - vertices[(i+1)%vertices.size()].second) - 
            (vertices[(i+1)%vertices.size()].second - vertices[i%vertices.size()].second) * 
            (vertices[(i+2)%vertices.size()].first - vertices[(i+1)%vertices.size()].first);
        if (cross1 * cross2 < 0)
            return false;
    }
    return true;
}

bool Source::check_point_in_polygon(const std::pair<int, int>& point, const std::vector<std::pair<int, int>>& vertices)
{
    int cross1, cross2;
    cross2 = (vertices[1].first - point.first) * (vertices[2].second - vertices[1].second) - 
        (vertices[1].second - point.second) * (vertices[2].first - vertices[1].first);
    for (size_t i = 1; i <= vertices.size(); ++i) {
        std::swap(cross1, cross2);
        cross2 = (vertices[(i+1)%vertices.size()].first - point.first) * 
            (vertices[(i+2)%vertices.size()].second - vertices[(i+1)%vertices.size()].second) - 
            (vertices[(i+1)%vertices.size()].second - point.second) * 
            (vertices[(i+2)%vertices.size()].first - vertices[(i+1)%vertices.size()].first);
        if (cross1 * cross2 < 0)
            return false;
    }
    return true;
}

bool Source::mark_points(const std::vector<std::pair<size_t, size_t>>& p)
{
    std::vector<std::pair<int, int>> vers(p.size());
    for (size_t i = 0; i < p.size(); ++i) {
        vers[i] = static_cast<std::pair<int, int>>(p[i]);
    }

    if (!check_polygon(vers))
        return false;

    auto row = [this, &vers](size_t i){
        for (size_t j = 0; j < field_m[i].size(); ++j)
            field_m[i][j] = check_point_in_polygon({i,j}, vers);
    };

    std::thread threads[max_threads_m];
    for (size_t i = 0; i < field_m.size(); i += max_threads_m) {
        for (size_t th = 0; th < max_threads_m; ++th)
            threads[th] = std::thread(row, i + th);
        for (size_t th = 0; th < max_threads_m; ++th)
            threads[th].join();
    }
    
    return true;
}