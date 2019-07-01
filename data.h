#pragma once

#include "header.h"

struct Data {
	double rank; double possibility; iCard* card;
	Data(iCard* card, double rank, double possibility);
};
typedef struct Data Data;
