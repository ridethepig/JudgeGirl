#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE* f_log;

void init_log();
void free_log();
void logger(const char * msg);
