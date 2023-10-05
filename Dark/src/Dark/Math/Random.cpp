#include <random>

namespace Dark
{
	namespace Math
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		int RandomInt(int from, int to)
		{
			std::uniform_int_distribution<int> num(from, to);
			return num(gen);
		}

		float RandomFloat(float from, float to)
		{
			std::uniform_real_distribution<float> num(from, to);
			return num(gen);
		}
	}
}