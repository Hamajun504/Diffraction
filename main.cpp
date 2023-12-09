#include "screen.cpp"
#include <fstream>
#include <iostream>

int main()
{
    Source source(200, 200, 0.001, 100e-9);
    Screen screen(200, 200, 20, source);

    source.mark_points({{50, 0}, {150, 0}, {200, 100}, {100, 200}, {0,100}});
    
    screen.evaluate_intensity();

    std::ofstream out("buffer_image.csv");
    auto image = screen.normalized_log_sqr_intensity();
    out << image;
    out.close();

    return 0;
}
