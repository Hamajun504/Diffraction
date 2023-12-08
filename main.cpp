#include "screen.cpp"
#include <fstream>
#include <iostream>

int main()
{
    Source source(300, 300, 0.001, 100e-9);
    Screen screen(200, 200, 10);

    auto& arr = source.array();

    source.mark_points({{0, 0}, {0, 300}, {30, 300}, {30, 0}});
    
    screen.evaluate_intensity(source);

    std::ofstream out("buffer_image.csv");
    auto image = screen.normalized_log_sqr_intensity();
    out << image;
    out.close();

    return 0;
}
