#include <vector>
#pragma once
using std::size_t;


class Source final
{
    std::vector<std::vector<bool>> field_m;
    std::pair<size_t, size_t> shape_m;
    std::pair<double, double> size_m;
    double wavelength_m;

public:
    Source(size_t Nx, size_t Ny, double, double);

    const std::pair<size_t, size_t>& shape() const;

    const std::pair<size_t, size_t> size() const;

    const double wavelength() const;

    const std::vector<bool>& operator[](size_t) const;

    std::vector<std::vector<bool>>& array() {return field_m;}

};

Source::Source(size_t Nx, size_t Ny, double width, double wavelength): 
    field_m(Nx, std::vector<bool>(Ny, false)), shape_m(Nx, Ny), size_m(width, Ny / Nx * width), wavelength_m(wavelength) {}

const std::pair<size_t, size_t>& Source::shape() const
{
    return shape_m;
}

const std::pair<size_t, size_t> Source::size() const
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