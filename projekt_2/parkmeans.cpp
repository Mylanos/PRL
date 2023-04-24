#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <list>
#include <cctype>
#include<mpi.h>
#include<bits/stdc++.h>

std::string FILE_NUMBERS = "numbers";

std::string read_file(std::string& file_path)
{
  std::fstream file;
  std::string content;
  file.open(file_path, std::ios::in);
  if (!file)
  {
    std::cerr << "Error parsing files: No such file";
    exit(1);
  }
  content.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  file.close();
  return content;
}

std::string get_input()
{
  std::string result;
  std::string path = "numbers";
  return read_file(path);
  
}

int main(int argc, char** argv)
{

  const int N = 100;
  int root = 0;
  MPI_Status status;
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &no_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_no);

  std::cout << no_of_processes << std:endl

  MPI_Finalize();
  return 0;
}
