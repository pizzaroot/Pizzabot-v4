#include <iostream>
#include "HAPIH.h"
#include <windows.h>
#include <cmath>
#include <vector>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

#define POPSIZE 50
#define NVARS 100

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
	int block_blocks[] = {1, 2, 3, 4, 6, 7, 40, 62, 65, 83};
	int block_obstacles[] = {8, 9, 39, 103};
	int block_orbs[] = {36, 84};
	int block_pads[] = {35, 67};

	std::ifstream level_file("level.txt");
	std::string str;
	std::string level;
	while (std::getline(level_file, str))
	{
		level = str;
	}

	std::istringstream ss(level);
	std::string token;

	std::vector<std::vector<float>> locblock_block;
	std::vector<std::vector<float>> locblock_obstacle;
	std::vector<std::vector<float>> locblock_orb;
	std::vector<std::vector<float>> locblock_pad;

	while (std::getline(ss, token, ';')) {
		std::istringstream ssblock(token);
		std::string token2;

		int i = 0;

		int type;
		float x;
		float y;

		while (std::getline(ssblock, token2, ',')) {
			if (i == 1) {
				type = std::stoi(token2);
			}
			else if (i == 3) {
				x = ::atof(token2.c_str());
			}
			else if (i == 5) {
				y = ::atof(token2.c_str());
			}

			i++;
		}

		std::vector<float> xy_array = {x, y};

		if (std::find(std::begin(block_blocks), std::end(block_blocks), type) != std::end(block_blocks)) {
			locblock_block.push_back(xy_array);
		}
		else if (std::find(std::begin(block_obstacles), std::end(block_obstacles), type) != std::end(block_obstacles)) {
			locblock_obstacle.push_back(xy_array);
		}
		else if (std::find(std::begin(block_orbs), std::end(block_orbs), type) != std::end(block_orbs)) {
			locblock_orb.push_back(xy_array);
		}
		else if (std::find(std::begin(block_pads), std::end(block_pads), type) != std::end(block_pads)) {
			locblock_pad.push_back(xy_array);
		}
	}

	std::cout << "Pizzabot v4.0\nAI that learns to play Geometry Dash\nMade by Pizzaroot\n" << std::endl;

	HackIH GD;

	int widthGD = 600;
	int heightGD = 400;

	float sum;
	float xPos;
	float yPos;

	srand(time(NULL));

	for (int i = 0; i < POPSIZE; i++) {
		for (int j = 0; j < NVARS; j++) {
			population[i].gene_p[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
			population[i].gene_n[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
		}
		population[i].fitness = 0;
		population[i].rfitness = 0;
	}

	float xMax = 0;
	float lastX = 0;
	float lastDeath = 0;
	int gen = 0;
	int pop = 0;

	int lastaction = 0;

	GD.bind("GeometryDash.exe");

	std::cout << "gen " << gen << " population " << pop << std::endl;

	do {
		xPos = GD.Read<float>({ GD.BaseAddress , 0x3222D0 , 0x164, 0x224, 0x67C });
		yPos = GD.Read<float>({ GD.BaseAddress , 0x3222D0 , 0x164, 0x38C, 0xB4, 0x224, 0x680 });

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
								population[i].gene_p[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
								population[i].gene_n[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
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
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_block.size(); i++) {
				if (locblock_block[i][0] - xPos + 200 < 600 && locblock_block[i][1] - yPos + 200 < 400 && locblock_block[i][0] - xPos + 200 - 15 < rx && rx < locblock_block[i][0] - xPos + 200 + 15 && locblock_block[i][1] - yPos + 200 - 15 < ry && ry < locblock_block[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
			for (int i = 0; i < locblock_obstacle.size(); i++) {
				if (locblock_obstacle[i][0] - xPos + 200 < 600 && locblock_obstacle[i][1] - yPos + 200 < 400 && locblock_obstacle[i][0] - xPos + 200 - 15 < rx && rx < locblock_obstacle[i][0] - xPos + 200 + 15 && locblock_obstacle[i][1] - yPos + 200 - 15 < ry && ry < locblock_obstacle[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
			for (int i = 0; i < locblock_orb.size(); i++) {
				if (locblock_orb[i][0] - xPos + 200 < 600 && locblock_orb[i][1] - yPos + 200 < 400 && locblock_orb[i][0] - xPos + 200 - 15 < rx && rx < locblock_orb[i][0] - xPos + 200 + 15 && locblock_orb[i][1] - yPos + 200 - 15 < ry && ry < locblock_orb[i][1] - yPos + 200 + 15) {
					sum += -1;
				}
			}
			for (int i = 0; i < locblock_pad.size(); i++) {
				if (locblock_pad[i][0] - xPos + 200 < 600 && locblock_pad[i][1] - yPos + 200 < 400 && locblock_pad[i][0] - xPos + 200 - 15 < rx && rx < locblock_pad[i][0] - xPos + 200 + 15 && locblock_pad[i][1] - yPos + 200 - 15 < ry && ry < locblock_pad[i][1] - yPos + 200 + 15) {
					sum += -1;
				}
			}
		}

		for (int xy : population[pop].gene_n) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_block.size(); i++) {
				if (locblock_block[i][0] - xPos + 200 < 600 && locblock_block[i][1] - yPos + 200 < 400 && locblock_block[i][0] - xPos + 200 - 15 < rx && rx < locblock_block[i][0] - xPos + 200 + 15 && locblock_block[i][1] - yPos + 200 - 15 < ry && ry < locblock_block[i][1] - yPos + 200 + 15) {
					sum += -1;
				}
			}
			for (int i = 0; i < locblock_obstacle.size(); i++) {
				if (locblock_obstacle[i][0] - xPos + 200 < 600 && locblock_obstacle[i][1] - yPos + 200 < 400 && locblock_obstacle[i][0] - xPos + 200 - 15 < rx && rx < locblock_obstacle[i][0] - xPos + 200 + 15 && locblock_obstacle[i][1] - yPos + 200 - 15 < ry && ry < locblock_obstacle[i][1] - yPos + 200 + 15) {
					sum += -1;
				}
			}
			for (int i = 0; i < locblock_orb.size(); i++) {
				if (locblock_orb[i][0] - xPos + 200 < 600 && locblock_orb[i][1] - yPos + 200 < 400 && locblock_orb[i][0] - xPos + 200 - 15 < rx && rx < locblock_orb[i][0] - xPos + 200 + 15 && locblock_orb[i][1] - yPos + 200 - 15 < ry && ry < locblock_orb[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
			for (int i = 0; i < locblock_pad.size(); i++) {
				if (locblock_pad[i][0] - xPos + 200 < 600 && locblock_pad[i][1] - yPos + 200 < 400 && locblock_pad[i][0] - xPos + 200 - 15 < rx && rx < locblock_pad[i][0] - xPos + 200 + 15 && locblock_pad[i][1] - yPos + 200 - 15 < ry && ry < locblock_pad[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
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
