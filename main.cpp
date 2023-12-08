#include "screen.cpp"
#include <fstream>
#include <iostream>

int main()
{
    Source source(150, 150, 0.001, 500e-9);
    Screen screen(100, 100, 5);

    auto& arr = source.array();

    for (size_t i = 0; i < arr.size(); ++i)
        for (size_t j = 0; j < arr[0].size(); ++j)
        {
            if (i >= 65 && i < 85)
                arr[i][j] = true;
        }
    
    screen.evaluate_phase(source);

    std::ofstream out("buffer_image.csv");
    auto image = screen.normalized_intensity();
    for (size_t i = 0; i < image.size(); ++i)
        for (size_t j = 0; j < image[0].size(); ++j) {
            out << image[i][j];
            if (j + 1 == image[0].size())
                out << std::endl;
            else
                out << ',';
        }
    out.close();

    return 0;
}
