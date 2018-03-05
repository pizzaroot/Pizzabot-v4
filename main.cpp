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
	int gene_p_blocks[NVARS];
	int gene_n_blocks[NVARS];
	int gene_p_obstacles[NVARS];
	int gene_n_obstacles[NVARS];
	int gene_p_orbs[NVARS];
	int gene_n_orbs[NVARS];
	int gene_p_pads[NVARS];
	int gene_n_pads[NVARS];
	float fitness;
	float rfitness;
};

struct genotype population[POPSIZE + 1];
struct genotype newpopulation[POPSIZE + 1];

int main()
{
	std::cout << "Pizzabot v4.0\nAI that learns to play Geometry Dash\nMade by Pizzaroot\n" << std::endl;

	int block_blocks[] = {1, 2, 3, 4, 6, 7, 40, 62, 65, 83};
	int block_obstacles[] = {8, 9, 39, 103, 144};
	int block_orbs[] = {36, 84};
	int block_pads[] = {35, 67};

	std::string levelstringfile;

	std::cout << "Import Level File: ";
	std::cin >> levelstringfile;

	std::ifstream level_file(levelstringfile);
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

	std::string databasefile;

	std::cout << "Database File: ";
	std::cin >> databasefile;

	std::ifstream data_file(databasefile);
	std::string str2;

	int i1 = 0;
	int j1 = 0;

	while (std::getline(data_file, str2))
	{
		if (str2 != "") {
			if (i1 < 50) {
				population[i1 % 50].gene_p_blocks[j1] = std::stoi(str2);
			}
			else if (i1 < 100) {
				population[i1 % 50].gene_n_blocks[j1] = std::stoi(str2);
			}
			else if (i1 < 150) {
				population[i1 % 50].gene_p_obstacles[j1] = std::stoi(str2);
			}
			else if (i1 < 200) {
				population[i1 % 50].gene_n_obstacles[j1] = std::stoi(str2);
			}
			else if (i1 < 250) {
				population[i1 % 50].gene_p_orbs[j1] = std::stoi(str2);
			}
			else if (i1 < 300) {
				population[i1 % 50].gene_n_orbs[j1] = std::stoi(str2);
			}
			else if (i1 < 350) {
				population[i1 % 50].gene_p_pads[j1] = std::stoi(str2);
			}
			else if (i1 < 400) {
				population[i1 % 50].gene_n_pads[j1] = std::stoi(str2);
			}
			j1++;
			if (j1 == 100) {
				i1++;
				j1 = 0;
			}
		}
	}

	data_file.close();

	HackIH GD;

	int widthGD = 600;
	int heightGD = 400;

	float sum;
	float xPos;
	float yPos;

	srand(time(NULL));

	if (i1 == 0) {
		for (int i = 0; i < POPSIZE; i++) {
			for (int j = 0; j < NVARS; j++) {
				population[i].gene_p_blocks[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
				population[i].gene_n_blocks[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
				population[i].gene_p_obstacles[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
				population[i].gene_n_obstacles[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
				population[i].gene_p_orbs[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
				population[i].gene_n_orbs[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
				population[i].gene_p_pads[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
				population[i].gene_n_pads[j] = (rand() % widthGD) * 500 + (rand() % heightGD);
			}
			population[i].fitness = 0;
			population[i].rfitness = 0;
		}
	}

	float xMax = 0;
	float lastX = 0;
	float lastDeath = 0;
	int gen = 0;
	int pop = 0;

	int lastaction = 0;

	GD.bind("GeometryDash.exe");

	std::cout << "gen " << gen << " population " << pop << std::endl;

	float startX = 0;

	do {
		xPos = GD.Read<float>({ GD.BaseAddress , 0x3222D0 , 0x164, 0x224, 0x67C });
		yPos = GD.Read<float>({ GD.BaseAddress , 0x3222D0 , 0x164, 0x38C, 0xB4, 0x224, 0x680 });

		if (lastX > xPos) { // new attempt
			population[pop].fitness = lastX - startX;

			startX = xPos;

			if (pop >= POPSIZE - 1) {
				int sorted_index[POPSIZE];
				int sorted_index_reverse[POPSIZE]; // index number of population's fitness

				for (int i = 0; i < POPSIZE; i++) {
					sorted_index[i] = i;
				}

				for (int i = 1; i < POPSIZE; i++) {
					for (int j = 0; j < POPSIZE - i; j++) {
						if (population[j].fitness > population[j + 1].fitness) {
							int temp_fit = population[j].fitness;
							population[j].fitness = population[j + 1].fitness;
							population[j + 1].fitness = temp_fit;
							int temp = sorted_index[j];
							sorted_index[j] = sorted_index[j + 1];
							sorted_index[j + 1] = temp;
						}
					}
				}

				for (int i = 0; i < POPSIZE; i++) {
					sorted_index_reverse[sorted_index[i]] = i;
				}

				for (int i = 0; i < POPSIZE; i++) {
					population[i].fitness = 0;

					int clear_count = 0;

					for (int j = 0; j < floor((POPSIZE - sorted_index_reverse[i] - 1) * NVARS / POPSIZE / 4); j++) {
						int random_index = rand() % NVARS;
						population[i].gene_p_blocks[random_index] = (rand() % widthGD) * 500 + (rand() % heightGD);
						population[i].gene_n_blocks[random_index] = (rand() % widthGD) * 500 + (rand() % heightGD);
						population[i].gene_p_obstacles[random_index] = (rand() % widthGD) * 500 + (rand() % heightGD);
						population[i].gene_p_obstacles[random_index] = (rand() % widthGD) * 500 + (rand() % heightGD);
						population[i].gene_p_orbs[random_index] = (rand() % widthGD) * 500 + (rand() % heightGD);
						population[i].gene_p_orbs[random_index] = (rand() % widthGD) * 500 + (rand() % heightGD);
						population[i].gene_p_pads[random_index] = (rand() % widthGD) * 500 + (rand() % heightGD);
						population[i].gene_p_pads[random_index] = (rand() % widthGD) * 500 + (rand() % heightGD);
						clear_count++;
					}

					for (int j = sorted_index_reverse[i] + 1; j < POPSIZE; j++) {
						for (int k = 0; k < sqrt(j); k++) {
							int random_index = rand() % NVARS;
							population[i].gene_p_blocks[random_index] = population[sorted_index[j]].gene_p_blocks[random_index];
							population[i].gene_n_blocks[random_index] = population[sorted_index[j]].gene_n_blocks[random_index];
							population[i].gene_p_obstacles[random_index] = population[sorted_index[j]].gene_p_obstacles[random_index];
							population[i].gene_p_obstacles[random_index] = population[sorted_index[j]].gene_p_obstacles[random_index];
							population[i].gene_p_orbs[random_index] = population[sorted_index[j]].gene_p_orbs[random_index];
							population[i].gene_p_orbs[random_index] = population[sorted_index[j]].gene_p_orbs[random_index];
							population[i].gene_p_pads[random_index] = population[sorted_index[j]].gene_p_pads[random_index];
							population[i].gene_p_pads[random_index] = population[sorted_index[j]].gene_p_pads[random_index];
							clear_count++;
						}
					}
				}

				std::ofstream outfile;

				outfile.open(databasefile);

				for (int i = 0; i < POPSIZE; i++) {
					for (int j = 0; j < NVARS; j++) {
						outfile << population[i].gene_p_blocks[j] << std::endl;
					}
				}
				for (int i = 0; i < POPSIZE; i++) {
					for (int j = 0; j < NVARS; j++) {
						outfile << population[i].gene_n_blocks[j] << std::endl;
					}
				}
				for (int i = 0; i < POPSIZE; i++) {
					for (int j = 0; j < NVARS; j++) {
						outfile << population[i].gene_p_obstacles[j] << std::endl;
					}
				}
				for (int i = 0; i < POPSIZE; i++) {
					for (int j = 0; j < NVARS; j++) {
						outfile << population[i].gene_n_obstacles[j] << std::endl;
					}
				}
				for (int i = 0; i < POPSIZE; i++) {
					for (int j = 0; j < NVARS; j++) {
						outfile << population[i].gene_p_orbs[j] << std::endl;
					}
				}
				for (int i = 0; i < POPSIZE; i++) {
					for (int j = 0; j < NVARS; j++) {
						outfile << population[i].gene_n_orbs[j] << std::endl;
					}
				}
				for (int i = 0; i < POPSIZE; i++) {
					for (int j = 0; j < NVARS; j++) {
						outfile << population[i].gene_p_pads[j] << std::endl;
					}
				}
				for (int i = 0; i < POPSIZE; i++) {
					for (int j = 0; j < NVARS; j++) {
						outfile << population[i].gene_n_pads[j] << std::endl;
					}
				}

				outfile.close();

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

		///////////////////////
		/* CALCULATE THE SUM */
		///////////////////////

		sum = 0;

		for (int xy : population[pop].gene_p_blocks) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_block.size(); i++) {
				if (locblock_block[i][0] - xPos + 200 < 600 && locblock_block[i][1] - yPos + 200 < 400 && locblock_block[i][0] - xPos + 200 - 15 < rx && rx < locblock_block[i][0] - xPos + 200 + 15 && locblock_block[i][1] - yPos + 200 - 15 < ry && ry < locblock_block[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
		}

		for (int xy : population[pop].gene_n_blocks) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_block.size(); i++) {
				if (locblock_block[i][0] - xPos + 200 < 600 && locblock_block[i][1] - yPos + 200 < 400 && locblock_block[i][0] - xPos + 200 - 15 < rx && rx < locblock_block[i][0] - xPos + 200 + 15 && locblock_block[i][1] - yPos + 200 - 15 < ry && ry < locblock_block[i][1] - yPos + 200 + 15) {
					sum += -1;
				}
			}
		}

		for (int xy : population[pop].gene_p_obstacles) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_obstacle.size(); i++) {
				if (locblock_obstacle[i][0] - xPos + 200 < 600 && locblock_obstacle[i][1] - yPos + 200 < 400 && locblock_obstacle[i][0] - xPos + 200 - 15 < rx && rx < locblock_obstacle[i][0] - xPos + 200 + 15 && locblock_obstacle[i][1] - yPos + 200 - 15 < ry && ry < locblock_obstacle[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
		}

		for (int xy : population[pop].gene_n_obstacles) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_obstacle.size(); i++) {
				if (locblock_obstacle[i][0] - xPos + 200 < 600 && locblock_obstacle[i][1] - yPos + 200 < 400 && locblock_obstacle[i][0] - xPos + 200 - 15 < rx && rx < locblock_obstacle[i][0] - xPos + 200 + 15 && locblock_obstacle[i][1] - yPos + 200 - 15 < ry && ry < locblock_obstacle[i][1] - yPos + 200 + 15) {
					sum += -1;
				}
			}
		}

		for (int xy : population[pop].gene_p_orbs) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_orb.size(); i++) {
				if (locblock_orb[i][0] - xPos + 200 < 600 && locblock_orb[i][1] - yPos + 200 < 400 && locblock_orb[i][0] - xPos + 200 - 15 < rx && rx < locblock_orb[i][0] - xPos + 200 + 15 && locblock_orb[i][1] - yPos + 200 - 15 < ry && ry < locblock_orb[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
		}

		for (int xy : population[pop].gene_n_orbs) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_orb.size(); i++) {
				if (locblock_orb[i][0] - xPos + 200 < 600 && locblock_orb[i][1] - yPos + 200 < 400 && locblock_orb[i][0] - xPos + 200 - 15 < rx && rx < locblock_orb[i][0] - xPos + 200 + 15 && locblock_orb[i][1] - yPos + 200 - 15 < ry && ry < locblock_orb[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
		}

		for (int xy : population[pop].gene_p_pads) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

			for (int i = 0; i < locblock_pad.size(); i++) {
				if (locblock_pad[i][0] - xPos + 200 < 600 && locblock_pad[i][1] - yPos + 200 < 400 && locblock_pad[i][0] - xPos + 200 - 15 < rx && rx < locblock_pad[i][0] - xPos + 200 + 15 && locblock_pad[i][1] - yPos + 200 - 15 < ry && ry < locblock_pad[i][1] - yPos + 200 + 15) {
					sum += 1;
				}
			}
		}

		for (int xy : population[pop].gene_n_pads) {
			int rx = floor(xy / 500);
			int ry = xy % 500;

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
