#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <array>


class Generator {
public:
    virtual double getNext() = 0;
};


class LinearCongruentialGenerator : public Generator {
private:
    unsigned long long m, a, c, x;

public:
    LinearCongruentialGenerator(unsigned long long m, unsigned long long a, unsigned long long c, unsigned long long seed) : m(m), a(a), c(c), x(seed) {}

    double getNext() override {
        x = (a * x + c) % m;
        return static_cast<double>(x) / m;
    }
};




class QuadraticCongruentialGenerator : public Generator {
private:
    unsigned long long m, a, c, d, x;

public:
    QuadraticCongruentialGenerator(unsigned long long m, unsigned long long a, unsigned long long c, unsigned long long d, unsigned long long seed) : m(m), a(a), c(c), d(d), x(seed) {}

    double getNext() override {
        x = (d * x * x + a * x + c) % m;
        return static_cast<double>(x) / m;
    }
};



class FibonacciGenerator : public Generator {
private:
    unsigned long long m;
    unsigned long long x1, x2;

public:
    FibonacciGenerator(unsigned long long m, unsigned long long seed) : m(m), x1(0), x2(1) {
        if (seed != 0) {
            x1 = seed;
        }
    }

    double getNext() override {
        unsigned long long nextValue = (x1 + x2) % m;

        x1 = x2;
        x2 = nextValue;

        return static_cast<double>(nextValue) / m;
    }
};



class InverseCongruentialGenerator : public Generator {
private:
    unsigned long long p, a, c, x;

    unsigned long long modInverse(unsigned long long a, unsigned long long m) {
        unsigned long long m0 = m;
        unsigned long long x0 = 0;
        unsigned long long x1 = 1;

        while (a > 1) {
            unsigned long long q = a / m;
            unsigned long long t = m;
            m = a % m;
            a = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }

        if (x1 < 0) {
            x1 += m0;
        }

        return x1;
    }

public:
    InverseCongruentialGenerator(unsigned long long p, unsigned long long a, unsigned long long c, unsigned long long seed) : p(p), a(a), c(c), x(seed) {}

    double getNext() override {
        unsigned long long inv_x = modInverse(x, p);

        x = (a * inv_x + c) % p;

        return static_cast<double>(x) / p;
    }
};



class CombineMethodgenerator : public Generator{
    Generator* X;
    Generator* Y;
public:
    CombineMethodgenerator(Generator* genX, Generator* genY) : X(genX), Y(genY) {}
    double getNext() override{
        double x = X->getNext();
        double y = Y->getNext();
        double difference = x - y;
        if (difference < 0.0) {
            difference += 1.0;
        }
        return difference;
    }
};



class ThreeSigmaGenerator : public Generator {
private:
    double m, s;

public:
    ThreeSigmaGenerator(double mean, double stddev)
        : m(mean), s(stddev) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    double getNext() override {
        double sum = 0.0;
        for (int i = 0; i < 12; ++i) {
            sum += static_cast<double>(std::rand()) / 2147483647;
        }

        double randomValue = m + (sum - 6.0) * s;

        return randomValue;
    }
};



class PolarCoordinateGenerator : public Generator {
public:
    double last = -1;
    double getNext() override {
        double v1, v2, s;
        if (last >0){
            double t = last;
            last = -1;
            return t;
        }else{
        do {
            v1 = 2.0 * static_cast<double>(std::rand()) / 2147483647;
            v2 = 2.0 * static_cast<double>(std::rand()) / 2147483647;
            s = v1 * v1 + v2 * v2;
        } while (s >= 1.0);

        double x1 = v1 * std::sqrt((-2.0 * std::log(s)) / s);
        last = v2 * std::sqrt((-2.0 * std::log(s)) / s);
        
        return x1;
        }
        
    }
};


void histogram(std::vector<double> randomValues, double minRange, double maxRange, int numIntervals) {
    std::vector<int> frequencyIntervals(numIntervals, 0);

    double intervalSize = (maxRange - minRange) / numIntervals;

    for (double randomValue : randomValues) {
        if (randomValue >= minRange && randomValue <= maxRange) {
            int interval = static_cast<int>((randomValue - minRange) / intervalSize);
            frequencyIntervals[interval]++;
        }
    }

    std::cout << "Interval   Frequency" << std::endl;
    for (int i = 0; i < numIntervals; ++i) {
        double start = minRange + i * intervalSize;
        double end = minRange + (i + 1) * intervalSize;
        double frequency = static_cast<double>(frequencyIntervals[i]) / randomValues.size();

        std::cout << "[" << start << "; " << end << "]    " << frequency << std::endl;
    }
}



int main() {
    std::vector<Generator*> generators;
    unsigned long long seed = static_cast<unsigned long long>(std::time(nullptr));

    generators.push_back(new LinearCongruentialGenerator(2147483647, 16807, 0, seed));
    generators.push_back(new QuadraticCongruentialGenerator(2147483647, 40014, 0, 53668, seed));
    generators.push_back(new FibonacciGenerator(2147483647, seed));
    generators.push_back(new InverseCongruentialGenerator(2147483647, 16805, 10, 1));
    generators.push_back(new CombineMethodgenerator(generators[0], generators[1]));
    generators.push_back(new ThreeSigmaGenerator(0, 1));
    generators.push_back(new PolarCoordinateGenerator());
    

    int choice;
    do {
        std::cout << "Choose a generator" << std::endl << "1: Linear" << std::endl << "2: Quadratic" << std::endl << "3: Fibonacci" << std::endl << "4: Inverse" << std::endl << "5: Combine" << std::endl << "6: Three sigma" << std::endl << "7: Polar coordinates" << std::endl << "0: Exit" << std::endl;
        std::cin >> choice;

        if (choice >= 1 && choice <= generators.size()) {
            
            Generator* selectedGenerator = generators[choice - 1];
            int n;
            std::cout << "Enter the number of random values to generate: ";
            std::cin >> n;
            
            int N;
            std::cout << "Enter the number of intervals for histogram: ";
            std::cin >> N;
            
            std::vector<double> randomValues;

            for (int i = 0; i < n; ++i) {
                double randomValue = selectedGenerator->getNext();
                randomValues.push_back(randomValue);
            }

            std::cout << "Random Values: ";
            for (int i = 0; i < randomValues.size(); ++i) {
            std::cout << randomValues[i];
            if (i < randomValues.size() - 1) {
            std::cout << ", ";
                }
            }
            
            std::cout<<std::endl;
            
            if (choice <= 5){
            histogram(randomValues, 0, 1, N);
            }else if ((choice > 5)){
                histogram(randomValues, -3, 3, N);
            }
        } else if (choice != 0) {
            std::cout << "Invalid choice. Please select a valid generator or 0 to exit." << std::endl;
        }
    } while (choice != 0);

    for (Generator* generator : generators) {
        delete generator;
    }

    return 0;
}
