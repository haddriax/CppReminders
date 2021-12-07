#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <random>
#include <chrono>
#include <future>
// #include <thread>

auto test1(int nb_iterations, const std::function<double()>& charge_algorithm) -> long long;
auto test2(int nb_iterations, const std::function<double()>& charge_algorithm) -> long long;

void run_tests();

// Number of times the charge algorithm is executed, also determine how many Threads are created.
constexpr int iterations = 256;

//  How heavy the charge algorithm will be.
constexpr int complexity = 150000;

constexpr int randMin = 1, randMax = 9;


int main()
{
	run_tests();
}

auto test1(const int nb_iterations, const std::function<double()>& charge_algorithm) -> long long
{
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();      

    std::vector< std::future<double> > threadsReturnValues;
    threadsReturnValues.reserve(nb_iterations);
    
    for (int x = 0; x < nb_iterations; x++)
    {
        threadsReturnValues.emplace_back(std::async(std::launch::async, charge_algorithm));
    }

    for (auto&& x : threadsReturnValues)
    {
        x.get();
    }

    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count());
}

auto test2(const int nb_iterations, const std::function<double()>& charge_algorithm) -> long long
{
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::vector<double> returnValues;
    returnValues.reserve(nb_iterations);

    for (int x = 0; x < iterations; x++)
    {
        returnValues.emplace_back(charge_algorithm());
    }

    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count());
}

void run_tests()
{
	auto input_data = std::make_shared<std::array<int, complexity>>();

    std::random_device r;
    const std::uniform_int_distribution<int> uniform_dist(randMin, randMax);
    std::default_random_engine e1(r());

    // Fill array with random double values.
    for (int& data : *(input_data.get()))
    {
        data = uniform_dist(e1);
    }

    // Processed lambda - inData as const should avoid any race condition here.
    auto process = [inData = std::as_const(input_data)]()-> float
    {
        float val = 0;

        for (int i = 0; i < complexity; ++i)
            val = sqrt(val + static_cast<float>(inData->at(i) * i));

        return val;
    };

    // Both tests return their execution duration.
    const long long t1ProcessDuration = test1(iterations, process); // With multi threading.
    const long long t2ProcessDuration = test2(iterations, process); // On the single main thread

    std::cout << "Process duration using multiple threads : "
        << t1ProcessDuration
        << "ms."
        << std::endl;

    std::cout << "Process duration using one thread : "
        << t2ProcessDuration
        << "ms."
        << std::endl;
}
