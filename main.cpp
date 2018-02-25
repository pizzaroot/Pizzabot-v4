#include <iostream>
#include "HAPIH.h"
#include <windows.h>
#include <cmath>
#include <vector>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <string>

int main()
{
	std::ifstream is_negative("negative.txt");
	std::istream_iterator<int> start_negative(is_negative), end_negative;
	std::vector<int> pixels_0(start_negative, end_negative);

	std::ifstream is_positive("positive.txt");
	std::istream_iterator<int> start_positive(is_positive), end_positive;
	std::vector<int> pixels_1(start_positive, end_positive);

	HackIH GD;

	RECT rectGD;

	HWND hGD = FindWindow(0, "Geometry Dash");
	
	GetWindowRect(hGD, &rectGD);

	int widthGD = rectGD.right - rectGD.left;
	int heightGD = rectGD.bottom - rectGD.top;

	float sum;
	float xPos;
	srand(time(NULL));

	float xMax = 0;
	int gen = pixels_0.size() + 1;
	int species = 1;
	float lastX = 0;
	float lastDeath = 0;

	//std::vector<int> pixels_0;
	//std::vector<int> pixels_1;

	HDC GDHDC = GetDC(hGD);

	//pixels_0.push_back(2148405);

	int lastaction = 0;

	//float macro[8192] = { 207.72, 259.65, 296.001, 311.58, 337.545, 353.124, 373.896, 394.668, 410.247, 425.826, 467.37, 482.949, 592.003, 659.514, 675.093, 693.252, 773.762, 815.307, 846.466, 864.045, 914.363, 965.714, 1023.03, 1077.35, 1158.05, 1189.2, 1241.13, 1272.29, 1308.64, 1334.6, 1420.65, 1496.35, 1537.13, 1578.67, 1656.56, 1680.3, 1724.07, 1744.84, 1786.38, 1807.16, 1827.93, 1843.51, 1885.05, 1905.82, 1942.17, 1957.75, 1994.1, 2009.68, 2046.03, 2071.99, 2108.35, 2134.31, 2612.1, 2648.46, 2690, 2710.78, 2736.74, 2754.75, 2779.86, 2796.6, 2821.71, 2842.64, 2867.76, 2876.13, 2892.87, 2905.43, 2922.17, 2930.54, 2943.1, 2951.47, 2980.77, 2984.95, 3018.44, 3043.55, 3068.66, 3099.22, 3130.38, 3208.28, 3319.24, 3338.61, 3697.39, 3806.59, 3861.19, 3900.18, 3923.58, 4056.66, 4095.66, 4134.66, 4173.67, 4197.07, 4314.08, 4423.29, 4493.5, 4540.3, 4626.11, 5021.19, 5044.59, 5067.99, 5083.59, 5122.6, 5146, 5224.01, 5239.61, 5302.01, 5317.62, 5372.22, 5419.03, 5458.03, 5653.05, 5676.45, 5707.65, 5738.86, 5777.86, 5801.26, 5855.87, 5879.27, 5926.08, 5957.28, 6288.61, 6507.04, 6600.65, 6616.25, 6702.06, 6754.71 };

	GD.bind("GeometryDash.exe");

	std::cout << "gen " << gen << " species " << species << " best " << xMax << std::endl;

	do {

		xPos = GD.Read<float>({ GD.BaseAddress , 0x3222D0 , 0x164, 0x224, 0x67C });
		
		if (lastX > xPos) { // new attempt
			species++;

			lastDeath = lastX;

			if (lastDeath > xMax) { // last death is the best
				gen++;
				species = 1;
				xMax = lastDeath;

				std::ofstream output_file_negative("negative.txt");
				std::ofstream output_file_positive("positive.txt");

				std::ostream_iterator<int> output_iterator_negative(output_file_negative, "\n");
				std::copy(pixels_0.begin(), pixels_0.end(), output_iterator_negative);
				std::ostream_iterator<int> output_iterator_positive(output_file_positive, "\n");
				std::copy(pixels_1.begin(), pixels_1.end(), output_iterator_positive);

				pixels_1.push_back((rand() % widthGD) * 4000 + (rand() % heightGD));
				pixels_0.push_back((rand() % widthGD) * 4000 + (rand() % heightGD));
			}
			else {
				pixels_1[pixels_1.size() - 1] = (rand() % widthGD) * 4000 + (rand() % heightGD);
				pixels_0[pixels_0.size() - 1] = (rand() % widthGD) * 4000 + (rand() % heightGD);
			}

			std::cout << "gen " << gen << " species " << species << " best " << xMax << std::endl;

			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

			lastaction = 0;
		}

		sum = 0;

		

		// connections
		for (int xy : pixels_0) {
			DWORD colour = GetPixel(GDHDC, floor(xy / 4000), xy % 4000);
			int r = GetRValue(colour);
			int g = GetGValue(colour);
			int b = GetBValue(colour);
			float neuron = (r + g + b - 383) / (float)383;
			sum += neuron * -1;
			//std::cout << "x: " << floor(xy / 4000) << ", y: " << xy % 4000 << ", rgb: (" << r << ", " << g << ", " << b << "), neuron: " << neuron << "" << std::endl;
		}

		for (int xy : pixels_1) {
			DWORD colour = GetPixel(GDHDC, floor(xy / 4000), xy % 4000);
			int r = GetRValue(colour);
			int g = GetGValue(colour);
			int b = GetBValue(colour);
			float neuron = (r + g + b - 383) / (float)383;
			sum += neuron * 1;
			//std::cout << "x: " << floor(xy / 4000) << ", y: " << xy % 4000 << ", rgb: (" << r << ", " << g << ", " << b << "), neuron: " << neuron << "" << std::endl;
		}

		if (sum > 0) {
			if (lastaction == 0) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				lastaction = 1;
			}
		}
		else {
			if (lastaction == 1) {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				lastaction = 0;
			}
		}

		//std::cout << sum << std::endl;

		lastX = xPos;
	} while (true);

	return 0;
}