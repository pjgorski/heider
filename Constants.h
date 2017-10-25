#pragma once

#include<vector>
#include<string>
#include<fstream>
#include<iostream>

const int BALANCE_CASE_COUNT = 8;
const bool LOUD = false;


const int UNSUPPORTED_GRAPH_TYPE = 2;
const int ZERO_TRIADS = 3;
const int NON_TRIAD = 4;
const int ERR_TRANSFORM = 5;
const int WRONG_CONF_FILE = 6;
const int ZERO_VECTOR_MEAN = 7;

static const char* file = "config.txt";

struct params{
	int I;
	int N_min, N_max, N_step;
	int attr_min, attr_max, attr_step;
	int maxIter;
	std::vector<std::pair<int, int>> printEvery;
	int printEveryLast;
	double p;
	std::string dynamicsType;
	std::string graphType;
};

bool read_flag(std::string flagName);

void read_conf(params& confParams);

static const bool TO_FILE = read_flag("TO_FILE");
static const bool ITER_KEEP = read_flag("ITER_KEEP");
static const bool INST_KEEP = read_flag("INST_KEEP");
static const bool FINAL_STATE_KEEP = read_flag("FINAL_STATE_KEEP");