#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <thread>
#include <future>
#include <random>
#include <chrono>

auto Test1(int iterations, const std::function<double()>& chargeAlgorithm) -> long long;
auto Test2(int iterations, const std::function<double()>& charge_algorithm) -> long long;

int main()
{

    constexpr int iterations = 10000;
    constexpr int complexity = 50000;

    constexpr int randMin = 1, randMax = 9;

	// Init rand.
    std::random_device r;
    std::uniform_int_distribution<int> uniform_dist(randMin, randMax);
    std::default_random_engine e1(r());

    std::array<double, complexity> inputDatas {};
    for (double& data : inputDatas)
    {
        data = uniform_dist(e1);
    }

    // Processed lambda
    auto process = [complexity, &inData = std::as_const(inputDatas)]()-> double
    {
        double val = 0;

        for (int i = 0; i < complexity; ++i)
            val += sqrt(inData.at(i) * i);

        return val;
    };

    const long long t1ProcessDuration = Test1(iterations, process);
    const long long t2ProcessDuration = Test2(iterations, process);

    std::cout << "Process duration using multiple threads : "
        << t1ProcessDuration
        << "ms."
        << std::endl;

    std::cout << "Process duration using one thread : "
        << t2ProcessDuration
        << "ms."
        << std::endl;
}

auto Test1(const int iterations, const std::function<double()>& chargeAlgorithm) -> long long
{
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();      

    std::vector< std::future<double> > threadsReturnValues;
    threadsReturnValues.reserve(iterations);
    
    for (int x = 0; x < iterations; x++)
    {
        threadsReturnValues.emplace_back(std::async(std::launch::async, chargeAlgorithm));
    }

    for (auto&& x : threadsReturnValues)
    {
        x.get();
    }

    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count());
}

auto Test2(const int iterations, const std::function<double()>& charge_algorithm) -> long long
{
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::vector<double> returnValues;
    returnValues.reserve(iterations);

    for (int x = 0; x < iterations; x++)
    {
        returnValues.emplace_back(charge_algorithm());
    }

    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count());
}