#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <random>

using namespace std;

class C
{
public:
    auto square(int x) -> int;
};

auto C::square(int x) -> int
{
    auto capturingLambda = [x](int value)-> int { return (value * x); };

    return capturingLambda(x);
}

int main()
{
    constexpr int randMin = 0, randMax = 9;

    // Init rand.
    std::random_device r;
    std::uniform_int_distribution<int> uniform_dist(randMin, randMax);
    std::default_random_engine e1(r());

    // Simple generci lambda.
    auto isquareLambda = [](int value) { return (value * value); };
    const int twoSquared = isquareLambda(2);
    cout << twoSquared << endl;

    // Code sample come from https://en.cppreference.com/w/cpp/language/lambda.

    auto vglambda = [](auto printer) {
        return [=](auto&&... ts) // Ts is a parameter pack // auto&& mean we accept both lvalue and rvalue // [=] mean we capture all the scope variables by value.
        {
            printer(forward<decltype(ts)>(ts)...);
            return [=] { printer(ts...); }; // nullary lambda (takes no parameters)
        };
    };
    auto p = vglambda([](auto v1, auto v2, auto v3) { cout << v1 << v2 << v3; }); // Create lambda, with an other lambda as parameter.
    auto q = p(1, 'a', 3.14); // outputs 1a3.14
    q();

    cout << endl;

    // Lambda capturing displacement variable. (C++14)

    auto multipliers = vector<int> { 1, 4, 6, 8 };
    auto pMultipliers = make_unique<vector<int>>(multipliers);
    auto nums = vector<int> { 3, 2, 1, 0 };

    // Capture Local variable pmultipliers, so it can be used inside the Lambda body.
    auto multiplyVector = [pointer = move(pMultipliers)](const vector<int>&& to_multiply)-> const vector<int>
    {
        vector<int> returnVector;
        returnVector.reserve(pointer->size() > to_multiply.size() ? pointer->size() : to_multiply.size());

        for (unsigned int i = 0; i < pointer->size(); ++i)
        {
            if ((i < pointer->size()) && (i < to_multiply.size()))
                returnVector.push_back(pointer->at(i) * to_multiply.at(i));
        }
    };

    // Capture the local rand generator.
    auto randSum = [uniform_dist, &e1]()-> int
    {
        int sum = 0;

        for (int i = 0; i < 100; ++i)
            sum += uniform_dist(e1);

        return sum;
    };

    std::mutex m;
    // Logging lambda
    auto logSumCallback = [&m](int loggedInt)-> void
    {
        std::lock_guard<std::mutex> lock(m);

        std::cout << "Log : " << std::this_thread::get_id() << " : " << loggedInt << std::endl;
    };

	constexpr int threadAmount = 50000;
    std::vector< std::future<int> > threadsReturnValues;
    threadsReturnValues.reserve(threadAmount);

    for (int i = 0; i < threadAmount; ++i)
    {
        threadsReturnValues.emplace_back(std::async(std::launch::async, randSum));
    }

    for (int i = 0; i < threadAmount; ++i)
    {
        // std::cout << threadsReturnValues[i].get() << ' ';
    }  

    return 0;
}
