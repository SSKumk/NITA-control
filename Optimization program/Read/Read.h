//
// Created by Антон on 07.07.2021.
//

#ifndef TIMES_AND_PLANES_OPTIMIZATION_2021_07_READ_H
#define TIMES_AND_PLANES_OPTIMIZATION_2021_07_READ_H

#include "string_view"

void read_checkPoints(std::string_view path);
void read_schemes(std::string_view path);

void openFile(const std::string_view &path, std::ifstream &openFile);
int count_number_of_line(std::string_view path);


#endif //TIMES_AND_PLANES_OPTIMIZATION_2021_07_READ_H
