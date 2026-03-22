#pragma once

#include <random>

class Random
{
  private:
    static inline std::random_device randomDevice;
    static inline std::mt19937 generator;

  public:
    static void initialize();
    [[nodiscard]] static float random(float min, float max);
    [[nodiscard]] static int randint(int min, int max);
};
