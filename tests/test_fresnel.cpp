#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <functional>
#include <vector>
#include "micromath.h"

const std::string file_extension = ".txt";

const float angle_step = 0.1;

void plot_data(std::ostream & os, const std::vector<float> & xdata, const std::function<float(float)> & y)
{
    for(auto x : xdata) {
        os << x << " " << y(x) << std::endl;
    }
}

void plot_data(const std::string & filename, const std::vector<float> & xdata, const std::function<float(float)> & y)
{
    std::cout << "plotting to " << filename << '\n';
    std::ofstream os(filename);
    plot_data(os, xdata, y);
}

std::vector<float> values_in_range(float start, float end, unsigned int num_points)
{
    std::vector<float> values;

    for(unsigned int i = 0; i < num_points; ++i) {
        values.push_back((end - start) * ((float) i / (num_points - 1)) + start);
    }

    return values;
}

std::vector<float> values_0_to_halfpi(unsigned int num_points)
{
    return values_in_range(0.0f, 0.5 * M_PI, num_points);
}

void plot_schlick(float F0)
{
    std::stringstream ss;
    ss << "fresnel_schlick_F" << std::fixed << std::setprecision(2) << F0 << "_v_angle"
        << file_extension;
    std::string filename = ss.str();

    plot_data(filename, values_0_to_halfpi(100), [&](float x) { return fresnel::schlick(F0, std::cos(x)); });
}

void plot_fresnel_dialectric_precise(float n_i, float n_t)
{
    {
        std::stringstream ss;
        ss << "fresnel_dialectric_unpolarized_ni_" << std::fixed << std::setprecision(2) << n_i
            << "_nt_" << std::fixed << std::setprecision(2) << n_t
            << file_extension;
        std::string filename = ss.str();

        plot_data(filename, values_0_to_halfpi(100),
                  [&](float angle_i) {
                    return fresnel::dialectric::unpolarizedSnell(std::cos(angle_i), n_i, n_t);
                  });
    }
    {
        std::stringstream ss;
        ss << "fresnel_dialectric_parallel_ni_" << std::fixed << std::setprecision(2) << n_i
            << "_nt_" << std::fixed << std::setprecision(2) << n_t
            << file_extension;
        std::string filename = ss.str();

        plot_data(filename, values_0_to_halfpi(100),
                  [&](float angle_i) {
                    return fresnel::dialectric::parallelSnell(std::cos(angle_i), n_i, n_t);
                  });
    }
    {
        std::stringstream ss;
        ss << "fresnel_dialectric_perpendicular_ni_" << std::fixed << std::setprecision(2) << n_i
            << "_nt_" << std::fixed << std::setprecision(2) << n_t
            << file_extension;
        std::string filename = ss.str();

        plot_data(filename, values_0_to_halfpi(100),
                  [&](float angle_i) {
                    return fresnel::dialectric::perpendicularSnell(std::cos(angle_i), n_i, n_t);
                  });
    }
}

void plot_snell(float n_i, float n_t)
{
    std::stringstream ss;
    ss << "snell_angle_ni_" << std::fixed << std::setprecision(2) << n_i
        << "_nt_" << std::fixed << std::setprecision(2) << n_t
        << file_extension;
    std::string filename = ss.str();

    plot_data(filename, values_0_to_halfpi(100), [&](float x) { return optics::snellsLawAngle(n_i, x, n_t); });
}

int main(int argc, char ** argv)
{
    auto F_0_to_1 = values_in_range(0.0, 1.0, 10);

    for(auto F : F_0_to_1) {
        plot_schlick(F);
    }

    auto iors = values_in_range(1.0, 2.42, 10);

    for(auto ior : iors) {
        plot_snell(1.0, ior);
        plot_snell(ior, 1.0);
    }

    plot_fresnel_dialectric_precise(1.0, 1.5);
    plot_fresnel_dialectric_precise(1.5, 1.0);

    return EXIT_SUCCESS;
}

