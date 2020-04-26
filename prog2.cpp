/*
FOR general version change largest_node_id_graph and smallest in line 280 and 287 to 0 edge_count_graph to 0 
Uncomment lines 74 to 97  
*/
#include<iostream>
#include<fstream>
#include<chrono>
#include<algorithm>
#include<cstring>
#include<iomanip>


// for memory mapping

#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <mpi.h>

using namespace std;
using namespace std:: chrono;

double print_time(time_point<high_resolution_clock>& start, time_point<high_resolution_clock>& end, 
                duration<double>& duration, int world_rank, ofstream &output){
                    
    duration = end - start;
    double duration_seconds = duration.count();
    double max_duration;
    MPI_Reduce(&duration_seconds, &max_duration, 1, MPI_DOUBLE, MPI_MAX,0, MPI_COMM_WORLD);
    return max_duration;
}

const char* get_file_map_info(const char* fname, size_t& num_bytes, int& world_rank, 
                                int& largest_node_id, int& smallest_node_id, int& edge_count){
    int fd = open(fname, O_RDONLY); // 19
    if(fd == -1){
        if (world_rank == 0){
            cerr<< "error opening the file" << endl;
        }
        return fname; // if there is an error, just returning the filename back
    }
    struct stat sb; // 20
    if(fstat(fd, &sb) == -1){
        if (world_rank == 0){
            cerr << " error in fstat " << endl ;
        }
        return fname;
    }
    num_bytes = sb.st_size;
    const char * addr = static_cast<const char*> (mmap(NULL, num_bytes,PROT_READ, MAP_PRIVATE, fd, 0u ));

    //addr[0], addr[1] give the first and 2nd character in file
    if(addr == MAP_FAILED){
        if(world_rank == 0){
            cerr << "mmap failed";
        }
        return fname;
    }

    //Getting the largest and smallest node value
    // use numb_bytes in loop condition
    char letter = '\0'; // 22
    int char_count = 0; // 23 keeps track of number of characters in each file
    char* buffer = new char[64](); // 26
    char* token = nullptr; // 27
    int node1, node2 = 0; // 28 29

    for (int i=0; i < num_bytes; i++){ // 21
        letter = addr[i];
        char_count++;
        if (letter == '\n'){
            edge_count++;
        
            for (int j = 0; j < char_count -1; j++){  // 25 char_count - 1 to remove the '\n at the end'
                buffer[j] = addr[i - (char_count -1) + j];
            }
            buffer[char_count - 1 ] = '\0';
            //cout << buffer << " lines " << edge_count;
            char_count = 0;

            // extacting 1st and 2nd string set characters from file
            token = strtok(buffer, "\t");
            node1 = atoi(token);
            token = strtok(NULL, "\t");
            node2 = atoi(token);
            //cout << " node1 " << node1 << " node2 "<< node2;

            // finding the largest and smallest nodeids
            if (node1 > largest_node_id)
                largest_node_id = node1;
            if (node2 > largest_node_id)
                largest_node_id = node2;
            
            if (node1 < smallest_node_id)
                smallest_node_id = node1;
            if (node2 < smallest_node_id)
                smallest_node_id = node2;
        }
    }

    delete [] buffer;
    return addr;
}

//pp is the partition pointer
void populate_neighbor_pid(const char* pp, size_t& num_bytes, int & world_rank, int& largest_node_id,
                                    int& smallest_node_id, int* NEIGH_COUNT, int* PROCESS_ID){
    
    char letter = '\0'; // 37
    int char_count = 0; // 38 keeps track of number of characters in each file
    char* buffer = new char[64](); // 39
    char* token = nullptr; // 27
    int node = 0, n_count = 0, p_id = 0, node_index = 0; // 40 41 45 46

    // use num_bytes in loop condition
    for (int i = 0; i < num_bytes; i++){ // 42
        letter = pp[i];
        char_count++;
        if (letter == '\n'){
        
            for (int j = 0; j < char_count -1; j++){  // 25 char_count - 1 to remove the '\n at the end'
                buffer[j] = pp[i - (char_count -1) + j];
            }
            buffer[char_count - 1 ] = '\0';
            //cout << buffer << " lines " << edge_count;
            char_count = 0;

            // extacting 1st and 2nd string set characters from file
            token = strtok(buffer, "\t");
            node = atoi(token);
            token = strtok(NULL, "\t");
            n_count = atoi(token);
            token = strtok(NULL, "\t");
            p_id = atoi(token);
            // if (world_rank == 0)
            //     cout << " node " << node << " n_count "<< n_count << " p_id " << p_id << endl;
            node_index = node - smallest_node_id + 1;
            NEIGH_COUNT[node_index] = n_count;
            PROCESS_ID[node_index] = p_id;
            
        }
    }
    // int counter = 0, break_point = 10; // 43 44
    // if (world_rank == 0){
    //     cout <<"neighbor count :" << "\n";
    //     for (int i = 1 ; i <= largest_node_id - smallest_node_id + 1; i++){
    //         cout << i << '\t' << NEIGH_COUNT[i];
    //         cout<< '\n';
    //         counter ++;
    //         if (counter == break_point){
    //             break;
    //         }
    //     }
    //     int counter = 0;
    //     cout << "process id :" << "\n";
    //     for (int i = 1 ; i <= largest_node_id - smallest_node_id + 1 ; i++){
    //         cout << i << '\t' << PROCESS_ID[i];
    //         cout<< '\n';
    //         counter ++;
    //         if (counter == break_point){
    //             break;
    //         }
    //     }
    // }
}

void populate_graph(const char* gp, size_t& num_bytes, int& world_rank, int& largest_node_id,
                    int& smallest_node_id, int& edge_count, int* EDGE_ARRAY){

    char letter = '\0'; // 47
    int char_count = 0; // 48 keeps track of number of characters in each file
    char* buffer = new char[64](); // 49
    char* token = nullptr; // 50
    int node1 = 0, node2 = 0, edge_index = 0, line_number = 0; // 51 52 53 54

    // use num_bytes in loop condition
    for (int i = 0; i < num_bytes; i++){ // 55
        letter = gp[i];
        char_count++;
        if (letter == '\n'){
        
            for (int j = 0; j < char_count -1; j++){  // 25 char_count - 1 to remove the '\n at the end'
                buffer[j] = gp[i - (char_count -1) + j];
            }
            buffer[char_count - 1 ] = '\0';
            //cout << buffer << " lines " << edge_count;
            char_count = 0;

            // extacting 1st and 2nd string set characters from file
            token = strtok(buffer, "\t");
            node1 = atoi(token);
            token = strtok(NULL, "\t");
            node2 = atoi(token);

            edge_index = 2 * line_number;
            EDGE_ARRAY[edge_index] = node1;
            EDGE_ARRAY[edge_index + 1] = node2;
            line_number ++;
        }
    }


    // if (world_rank == 0 ){
    //     cout << "Edges :" << endl;
    //     int counter = 0, break_point = 10; // 56 57
    //     for(int i = edge_count - 10 ; i < edge_count; i++ ){ // 58
    //         cout << EDGE_ARRAY[2*i] << '\t' <<EDGE_ARRAY[2*i + 1] << '\n';
    //         counter++;
    //         if (counter == break_point){
    //             break;
    //         }
    //     }
    // }
}


int main(int argc, char * argv[]){
    time_point<high_resolution_clock> start, end, start_init, end_init;
    start_init = high_resolution_clock::now();

    chrono::duration<double> duration;
    start = high_resolution_clock::now();
    cout << std::fixed;
    cout << std::setprecision(6);

    //Initialize the MPI environment
    // calling the MPI init function with address of argc and argv
    MPI_Init(&argc, &argv);

    // Get the number of processes
    // world size is upodated to the number of processes ??
    int world_size; // 1
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //Get the rank of the process
    int world_rank; // 2
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of th processor
    char processor_name[MPI_MAX_PROCESSOR_NAME]; // 3
    int name_len; // 4
    MPI_Get_processor_name(processor_name, &name_len);
    //MPI_Barrier(MPI_COMM_WORLD);

    // writing all the cout and print statements to ouptut files
    std::string filename = "out" + to_string(world_rank)+".txt"; // 5
    ofstream output; // 6
    output.open(filename, ios::out);
    /*
    Possible errors that could occur:
    1. Some extra node id present in graph and not in partition file
        - compare the largest and smallest nodes.
        - Even if there are some mismatching nodes in the middle, some memory might be allocated
        - errors will be generated only in those nodes i guess. 

    */
    if (argc != 5){
        //output << " $ usage: prog GraphFile PartitionFile Rounds Partitions" << endl;
        if (world_rank == 0){
            
            cout << " $ usage: prog GraphFile PartitionFile Rounds Partitions" << endl;   
        }
        return -1;
    }

    const char* graph = argv[1]; // 7  graph filename string not working for mmap
    const char* partition = argv[2]; // 8 partition filename
    int rounds = atoi(argv[3]); // 9
    int partitions = atoi(argv[4]); // 10
    int i, j, k = 0; // 11 12 13

    if (world_size != partitions){
        if(world_rank ==0){
        cout << "number of partitions " << partitions  << " not matching world_size " << world_size << endl;
        }
        return -1;
    }

    // Memory mapping both the graph and the partition file
    size_t num_bytes_graph = 0; // 14 
    int largest_node_id_graph = 1624992, smallest_node_id_graph = 1, edge_count_graph = 20000000; // 16 17 24
    const char *graph_pointer  = get_file_map_info(graph, num_bytes_graph, world_rank, 
                                                    largest_node_id_graph, smallest_node_id_graph, edge_count_graph); // 18
    

    size_t num_bytes_partition = 0; // 15
    int largest_node_id_partition = 1624992, smallest_node_id_partition = 1, num_lines_partition = 0; //30 31 32
    const char *partition_pointer = get_file_map_info(partition, num_bytes_partition, world_rank, 
                                                        largest_node_id_partition, smallest_node_id_partition,num_lines_partition); // 33

    // Error checking for file reading graph and partition file
    if (strlen(graph_pointer) == strlen(graph)){
        if (world_rank == 0){
        cout << "Error reading the file. Check the following : " << graph_pointer<<endl;
        cout << "\t Filename is in the same directory as the executable file " << endl;
        cout << "\t Filename matches the name in command line arguments " << endl;
        cout << "\t File is in a readble format by c++"<< endl;
        }
        return -1;
    }

    if (strlen(partition_pointer) == strlen(partition)){
        if(world_rank ==0){
        cout << "Error reading the file. Check the following : " << partition_pointer<<endl;
        cout << "\t Filename is in the same directory as the executable file " << endl;
        cout << "\t Filename matches the name in command line arguments " << endl;
        cout << "\t File is in a readble format by c++"<< endl;
        }
        return -1;
    }
    // if (world_rank == 0){
    // cout << "num bytes in graph " << num_bytes_graph << endl;
    // cout << "edge count graph " << edge_count_graph << endl;
    // cout << "largest node id graph " << largest_node_id_graph << endl;
    // cout << "smallest node id graph " << smallest_node_id_graph << endl;

    // cout << "num bytes in partition " << num_bytes_partition << endl;
    // cout << "num lines partition " << num_lines_partition << endl;
    // cout << "largest node id partition " << largest_node_id_partition << endl;
    // cout << "smallest node id partition " << smallest_node_id_partition << endl;
    // }

    /*
     Allocating the right amount of memory for 1D array based on largest and smallest node id
     Populating the arrays of neighbors count for each node 
     Populating which process each node belongs to
     Reading the the entire graph into a 1D array : GRAPH ARRAY 
     COMMON FOR ALL PARTITIONS 
     Plus 2 for ignoring 0th index and  (largest and smallest node id : 100 - 1 + 1)
    */
    int unique_node_range = largest_node_id_graph - smallest_node_id_graph + 2; // 34
    int *NEIGH_COUNT = new int[unique_node_range](); // 33
    //output << "Memory allocated for Neighbors Count Array "<<unique_node_range * sizeof(int)<< endl;
    int *PROCESS_ID = new int[unique_node_range]();// 35 
    int *EDGE_ARRAY = new int[edge_count_graph * 2](); // 36 
    double *R_COUNT = new double[unique_node_range](); //69


    
    populate_neighbor_pid(partition_pointer, num_bytes_partition, world_rank, largest_node_id_partition,
                            smallest_node_id_partition ,NEIGH_COUNT, PROCESS_ID);
    
    populate_graph(graph_pointer, num_bytes_graph, world_rank, largest_node_id_graph,
                        smallest_node_id_graph, edge_count_graph, EDGE_ARRAY);

    // storing the reciprocal of NEIGH_COUNT array
    for (int i = 0; i <= largest_node_id_graph; i++){
        if(NEIGH_COUNT[i] > 0){
            R_COUNT[i] = (double) 1/NEIGH_COUNT[i];
        }

    }

    
    end = high_resolution_clock::now();
    double duration_read = 0;
    duration_read = print_time(start, end, duration, world_rank, output);

    for (int i=0; i<world_size; i++){
        if(i == world_rank){
            cout << "time to read input files by partition " << i << " = "<<duration.count() <<"sec"<<endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank == 0){
            cout << "Total time to read input files by all partitions = " << duration_read << "sec" <<endl << endl;
    }



    
    // All arrays populated. Need to calculate credits
    // +1 for the 0th index. Not storing anything in the zeroth index
    //start = high_resolution_clock::now(); // 60
    int credit_array_size = (largest_node_id_graph * (rounds+1)) + 1; // 62
    double* CREDIT_LOCAL = new double[credit_array_size ]();
    double* CREDIT_GLOBAL = new double[credit_array_size]();
    int round_offset = 0, round_offset_one = 0, node1 = 0, node2 = 0, edge_list_offset = 0; // 69 70 71 72 73

    for(int i = 1; i <= largest_node_id_graph; i++ ){ //68
        //if(world_rank == PROCESS_ID[i])
            CREDIT_GLOBAL[i] = 1;
    }





    for (int round = 1; round <= rounds; round++){
        start = high_resolution_clock::now(); // 60
        round_offset = round*largest_node_id_graph;
        round_offset_one = (round-1) * largest_node_id_graph;
        

        for(int i = 0; i < edge_count_graph; i++){
            edge_list_offset = 2*i;
            node1 = EDGE_ARRAY[edge_list_offset];
            node2 = EDGE_ARRAY[edge_list_offset + 1];

            
            if(world_rank == PROCESS_ID[node1]){
                CREDIT_LOCAL[round_offset + node1] += R_COUNT[node2] * CREDIT_GLOBAL[round_offset_one + node2]; 
            }
            if(world_rank == PROCESS_ID[node2]){
                CREDIT_LOCAL[round_offset + node2] += R_COUNT[node1] * CREDIT_GLOBAL[round_offset_one + node1];
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allreduce(CREDIT_LOCAL, CREDIT_GLOBAL ,credit_array_size,MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        end = high_resolution_clock::now();
        double duration_round = 0;
        duration_round = print_time(start, end, duration, world_rank, output);

        for (int i=0; i < world_size; i++){
        if(i == world_rank){
            cout << "time for round " << round << ", partition "<< i << " = " <<duration.count() <<"sec"<<endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        if(world_rank == 0){
                cout << "Total time for round "<< round <<" by all partitions = " << duration_round << "sec" <<endl << endl;
        }
    }

    // CREDITS CALCULATED
    // WRITING TO FILES FROM CREDIT_LOCAL

    start = high_resolution_clock::now();
    output<<fixed << setprecision(6);
    
    for(int i = 1 ; i <= largest_node_id_graph; i++){
        
        if(world_rank == PROCESS_ID[i]){
            output << i ;
            for(int j = 1; j <= rounds; j++){
                output << '\t' << CREDIT_GLOBAL[j*largest_node_id_graph + i]; 
            }
            output << '\n';
        }
    }

    end = high_resolution_clock::now();
    double duration_write = 0;
    duration_write = print_time(start, end, duration, world_rank, output);
    for (int i=0; i<world_size; i++){
        if(i == world_rank){
            cout << "time to write output files by partition " << i << " = "<<duration.count() <<"sec"<<endl;
        }
       MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank == 0){
            cout << "Total time to write output files by all partitions = " << duration_write << "sec" <<endl << endl;
    }



    //Printing the first 15 values of round1 process 1 from CREDIT_LOCAL

    // if (world_rank == 1 ){
    //     cout << "CREDITS LOCAL:" << endl;
    //     int counter = 0, break_point = 15; // 62 63
    //     for(int i = round_offset + 1 ; i <= round_offset + 15; i++ ){ // 64
    //         cout << i - round_offset<< '\t' << CREDIT_LOCAL[i] << '\n';
    //         counter++;
    //         if (counter == break_point){
    //             break;
    //         }

    //     }
    // }
    

    //Printing the first 15 values of round1 from CREDIT_GLOBAL

    // if (world_rank == 0 ){
    //     cout << "CREDITS GLOBAL:" << endl;
    //     int counter = 0, break_point = 15; // 65 66
    //     for(int i = round_offset + 1 ; i <= round_offset + 15; i++ ){ // 67
    //         cout << i - round_offset << '\t' << CREDIT_GLOBAL[i] << '\n';
    //         counter++;
    //         if (counter == break_point){
    //             break;
    //         }

    //     }
    // }



    end = high_resolution_clock::now();
    print_time(start, end, duration, world_rank, output);

    
    //MPI_Barrier(MPI_COMM_WORLD);
    delete [] NEIGH_COUNT;
    delete [] PROCESS_ID;
    delete [] EDGE_ARRAY;
    delete [] CREDIT_LOCAL;
    delete [] CREDIT_GLOBAL;

    end_init = high_resolution_clock::now();
    double duration_end = 0;
    duration_end = print_time(start_init, end_init, duration, world_rank, output);
    if (world_rank == 0){
        cout << "Time to complete all processes " << duration_end << "sec"<< endl;
    }
    
    MPI_Finalize(); 

}