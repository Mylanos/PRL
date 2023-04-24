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
  char greeting[N];
  int no_of_processes, process_no, input_len, middle_number;
  MPI_Status status;
  int elements_per_proc, no_of_numbers;
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &no_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_no);
  u_int8_t* numbers = nullptr;

  if (process_no == 0){
    std::string line = get_input();
    input_len = line.size();
    numbers = new u_int8_t[input_len + 1];
    int middle_index;
    if ( input_len % 2 == 0)
      middle_index = (int) (input_len / 2) - 1;
    else
      middle_index = (int) (input_len / 2);
    std::cout << "INPUT: [";
    for(no_of_numbers = 0; no_of_numbers < input_len; no_of_numbers++){
      numbers[no_of_numbers] = unsigned(line[no_of_numbers]);
      if(no_of_numbers == input_len - 1){
        std::cout << unsigned(numbers[no_of_numbers]);
      }
      else{
        std::cout << unsigned(numbers[no_of_numbers]) << ",";
      }
    }
    elements_per_proc = (int) no_of_numbers / no_of_processes;
    std::cout << "]\n";
    middle_number = unsigned(numbers[middle_index]);
    std::cout << "MEDIAN: " << middle_number << std::endl;
  }

  MPI_Bcast( &elements_per_proc, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Bcast( &middle_number, 1, MPI_INT, root, MPI_COMM_WORLD);

  u_int8_t sub_numbers[elements_per_proc];

  MPI_Scatter(numbers, elements_per_proc, MPI_UINT8_T, sub_numbers, elements_per_proc, MPI_UINT8_T, 0, MPI_COMM_WORLD);

  u_int8_t tmp_subL[elements_per_proc], tmp_subE[elements_per_proc], tmp_subG[elements_per_proc];
  int no_Ls = 0;
  int no_Es = 0; 
  int no_Gs = 0;

  //printf("Process %d = [", process_no);
  for(int q = 0; q < elements_per_proc; q++){
    //printf("%d, ", sub_numbers[q]);
    if(sub_numbers[q] < middle_number){
      tmp_subL[no_Ls] = sub_numbers[q];
      no_Ls++;
    }
    else if(sub_numbers[q] > middle_number){
      tmp_subG[no_Gs] = sub_numbers[q];
      no_Gs++;
    }
    else{
      tmp_subE[no_Es] = sub_numbers[q];
      no_Es++;
    }
  }
  //printf("]\n");
  u_int8_t subsequence_L[no_Ls];
  u_int8_t subsequence_E[no_Es];
  u_int8_t subsequence_G[no_Gs];
  memcpy(subsequence_L, tmp_subL, no_Ls * sizeof(u_int8_t));
  memcpy(subsequence_E, tmp_subE, no_Es * sizeof(u_int8_t));
  memcpy(subsequence_G, tmp_subG, no_Gs * sizeof(u_int8_t));

  int *recvcounts_L = NULL;
  int *recvcounts_E = NULL;
  int *recvcounts_G = NULL;

  if (process_no == root){
    recvcounts_L = new int[no_of_processes];
    recvcounts_E = new int[no_of_processes];
    recvcounts_G = new int[no_of_processes];
  }

  MPI_Gather(&no_Ls, 1, MPI_INT, recvcounts_L, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Gather(&no_Es, 1, MPI_INT, recvcounts_E, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Gather(&no_Gs, 1, MPI_INT, recvcounts_G, 1, MPI_INT, root, MPI_COMM_WORLD);
  //MPI_Gatherv(&tmp_subE, 1, MPI_INT, E, 1, MPI_INT, root, MPI_COMM_WORLD);
  //MPI_Gather(&tmp_subG, 1, MPI_INT, G, 1, MPI_INT, root, MPI_COMM_WORLD);
  
  u_int8_t* L = nullptr;
  u_int8_t* E = nullptr;
  u_int8_t* G = nullptr;

  int* displs_L = nullptr;
  int* displs_E = nullptr;
  int* displs_G = nullptr;
  int total_len_L = 0;
  int total_len_E = 0;
  int total_len_G = 0;

  if (process_no == 0){
      displs_L = new int[no_of_processes];
      displs_E = new int[no_of_processes];
      displs_G = new int[no_of_processes];
      displs_L[0] = 0;
      displs_E[0] = 0;
      displs_G[0] = 0;
      total_len_L += recvcounts_L[0]; 
      total_len_E += recvcounts_E[0]; 
      total_len_G += recvcounts_G[0]; 

      for (int i=1; i<no_of_processes; i++) {
           total_len_L += recvcounts_L[i];
           displs_L[i] = displs_L[i-1] + recvcounts_L[i-1];
           total_len_E += recvcounts_E[i]; 
           displs_E[i] = displs_E[i-1] + recvcounts_E[i-1];
           total_len_G += recvcounts_G[i]; 
           displs_G[i] = displs_G[i-1] + recvcounts_G[i-1];
      }

      L = new u_int8_t[total_len_L];
      E = new u_int8_t[total_len_E];
      G = new u_int8_t[total_len_G];
      
      /*
      for (int q = 0; q < no_of_processes; q++){
        printf("PROC NO: %d (INDEX: %d) - L=%d, E=%d, G=%d \n", q, displs_E[q], recvcounts_L[q], recvcounts_E[q], recvcounts_G[q]);
      }
      */

  }

  MPI_Gatherv(subsequence_L, no_Ls, MPI_UINT8_T, L, recvcounts_L, displs_L, MPI_UINT8_T, root, MPI_COMM_WORLD);
  MPI_Gatherv(subsequence_E, no_Es, MPI_UINT8_T, E, recvcounts_E, displs_E, MPI_UINT8_T, root, MPI_COMM_WORLD);
  MPI_Gatherv(subsequence_G, no_Gs, MPI_UINT8_T, G, recvcounts_G, displs_G, MPI_UINT8_T, root, MPI_COMM_WORLD);

  if (process_no == root) {
    printf("L: [");
    for (int q = 0; q < total_len_L; q++){
      if(q == total_len_L - 1)
        printf("%d", L[q]);
      else
        printf("%d, ", L[q]);
    }
    printf("]\nE: [");
    for (int q = 0; q < total_len_E; q++){
      if(q == total_len_E - 1)
        printf("%d", E[q]);
      else
        printf("%d, ", E[q]);
    }
    printf("]\nG: [");
    for (int q = 0; q < total_len_G; q++){
      if(q == total_len_G - 1)
        printf("%d", G[q]);
      else
        printf("%d, ", G[q]);
    }
    printf("]\n");
  }

  MPI_Finalize();
  return 0;
}
