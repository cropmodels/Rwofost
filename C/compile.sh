#!/bin/bash
g++  -std=c++11 -O0 -g -I ../src/ date.cpp files.cpp ../src/astro.cpp ../src/cropsi.cpp ../src/evtra.cpp ../src/penman.cpp ../src/rootd.cpp ../src/soil.cpp ../src/stday.cpp ../src/subsol.cpp ../src/totass.cpp ../src/vernalisation.cpp ../src/watfd.cpp ../src/watgw.cpp ../src/watpp.cpp  ../src/wofost.cpp main.cpp -o WOFOST

#g++  -std=c++11 -I ../src/ date.cpp files.cpp ../src/astro.cpp ../src/cropsi.cpp ../src/evtra.cpp ../src/penman.cpp ../src/rootd.cpp ../src/soil.cpp ../src/stday.cpp ../src/subsol.cpp ../src/totass.cpp ../src/vernalisation.cpp ../src/watfd.cpp ../src/watgw.cpp ../src/watpp.cpp  ../src/wofost.cpp main.cpp -o WOFOST
#valgrind --leak-check=yes ./WOFOST

#../src/npk_demand_uptake.cpp ../src/npk_dynamics.cpp ../src/npk_soil_dynamics.cpp ../src/npk_translocation.cpp ../src/npk_stress.cpp
