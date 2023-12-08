#include "screen.cpp"

int main()
{
    Source source(150, 150, 0.01, 500e-9);
    Screen screen(100, 100, 10);

    auto& arr = source.array();

    for (size_t i = 0; i < arr.size(); ++i)
        for (size_t j = 0; j < arr[0].size(); ++j)
        {
            if (i >= 65 && i < 85)
                arr[i][j] = true;
        }
    
    screen.evaluate_phase(source);


    return 0;
}
