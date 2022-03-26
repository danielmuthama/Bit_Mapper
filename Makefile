CXX=g++

CXXFLAGS=-std=c++11


pagingwithtable : main.o PAGETable.o Map.o level.o tlb.o tracereader.o output_mode_helpers.o
	$(CXX) $(CXXFLAGS) -g -o pagingwithtable $^

main.o : main.cpp
	$(CXX) $(CXXFLAGS) -g -c $<

PAGETable.o : PAGETable.cpp PAGETable.hpp
	$(CXX) $(CXXFLAGS) -g -c $<

Map.o : Map.cpp Map.hpp
	$(CXX) $(CXXFLAGS) -g -c $<

level.o : level.cpp level.hpp
	$(CXX) $(CXXFLAGS) -g -c $<

tlb.o : tlb.cpp tlb.hpp
	$(CXX) $(CXXFLAGS) -g -c $<

tracereader.o : tracereader.c tracereader.h
	$(CXX) $(CXXFLAGS) -g -c $<

output_mode_helpers.o : output_mode_helpers.c output_mode_helpers.h
	$(CXX) $(CXXFLAGS) -g -c $<

clean :
	rm -f *.o