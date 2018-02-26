#include <iostream>
#include "HAPIH.h"
#include <windows.h>
#include <cmath>
#include <vector>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

#define POPSIZE 50
#define MAXGENS 5000
#define NVARS 10

struct genotype
{
	int gene_p[NVARS];
	int gene_n[NVARS];
	float fitness;
	float rfitness;
};

struct genotype population[POPSIZE + 1];
struct genotype newpopulation[POPSIZE + 1];

int main()
{
	std::cout << "Pizzabot v4.0\nAI that learns to play Geometry Dash\nMade by Pizzaroot\n" << std::endl;

	HackIH GD;

	RECT rectGD;

	HWND hGD = FindWindow(0, "Geometry Dash");
	
	GetWindowRect(hGD, &rectGD);

	int widthGD = rectGD.right - rectGD.left;
	int heightGD = rectGD.bottom - rectGD.top;

	float sum;
	float xPos;

	srand(time(NULL));

	for (int i = 0; i < POPSIZE; i++) {
		for (int j = 0; j < NVARS; j++) {
			population[i].gene_p[j] = (rand() % widthGD) * 4000 + (rand() % heightGD);
			population[i].gene_n[j] = (rand() % widthGD) * 4000 + (rand() % heightGD);
		}
		population[i].fitness = 0;
		population[i].rfitness = 0;
	}

	float xMax = 0;
	float lastX = 0;
	float lastDeath = 0;
	int gen = 0;
	int pop = 0;

	HDC GDHDC = GetDC(hGD);

	int lastaction = 0;

	GD.bind("GeometryDash.exe");

	std::cout << "gen " << gen << " population " << pop << std::endl;

	do {
		xPos = GD.Read<float>({ GD.BaseAddress , 0x3222D0 , 0x164, 0x224, 0x67C });
		
		if (lastX > xPos) { // new attempt
			population[pop].fitness = lastX;

			if (pop >= POPSIZE - 1) {
				float fitsum = 0;
				float fitmax = 0;
				int fitmaxindex = 0;

				for (int i = 0; i < POPSIZE; i++) {
					fitsum += population[i].fitness;
					if (population[i].fitness > fitmax) {
						fitmax = population[i].fitness;
						fitmaxindex = i;
					}
				}

				for (int i = 0; i < POPSIZE; i++) {
					population[i].rfitness = population[i].fitness / fitsum * POPSIZE;
					population[i].fitness = 0;
					if (population[i].rfitness <= 1) {
						for (int j = 0; j < NVARS; j++) {
							if (rand() % 2 == 0) {
								population[i].gene_p[j] = (rand() % widthGD) * 4000 + (rand() % heightGD);
								population[i].gene_n[j] = (rand() % widthGD) * 4000 + (rand() % heightGD);
							}
							else {
								population[i].gene_p[j] = population[fitmaxindex].gene_p[j];
								population[i].gene_n[j] = population[fitmaxindex].gene_n[j];
							}
						} 
					}
				}

				gen++;
				pop = 0;
			}
			else {
				pop++;
			}

			std::cout << "gen " << gen << " population " << pop << std::endl;

			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

			lastaction = 0;
		}

		sum = 0;

		for (int xy : population[pop].gene_p) {
			DWORD colour = GetPixel(GDHDC, floor(xy / 4000), xy % 4000);
			int r = GetRValue(colour);
			int g = GetGValue(colour);
			int b = GetBValue(colour);
			float neuron = (r + g + b - 383) / (float)383;
			sum += neuron * -1;
		}

		for (int xy : population[pop].gene_n) {
			DWORD colour = GetPixel(GDHDC, floor(xy / 4000), xy % 4000);
			int r = GetRValue(colour);
			int g = GetGValue(colour);
			int b = GetBValue(colour);
			float neuron = (r + g + b - 383) / (float)383;
			sum += neuron * 1;
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

		lastX = xPos;
	} while (true);

	return 0;
}
