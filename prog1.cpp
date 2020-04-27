/*
 Finding the page rank of a large graph. 
 Text file is a an egde list
 Need to read a large file using memory mapping 
 - Read one line at a time.
 - 
 Need to convert the convert the file into a adjacency matrix or adjacency list
    - store the matrix in an array in the flattened form.
    - 
 calculate the page rank using neighbors
    - For each node find its neighbors 
    - Calculate its pagerank.
    - Keep track of the credit for each node in separate data structure?
    - 

*/
/*
QUESTIONS
Are all line in the text file ended with '\0' ??

*/

// NOTE: CODE WORKED AFTER CONVERTING VALUES OF PARTITION GRAPH TO STRING !! NOT IT DIDN'T
// NOTE: I THINK THE ISSUE WITH THE GRAPH FILE ITSELF
// NOTE: LOOKS LIKE THE GRAPH FILE IS SEPERATED BY SPACES AND NOT '\t' FOR FACEBOOK GRAPH
// NOTE: WILL WRITE THE ENTIRE GRAPH TO ANOTHER FILE AND TRY IT AGAIN
// NOTE: THE ISSUE WAS WITH THE SPACING BETWEEN EACH NODE. IT WAS SPACE INSTEAD OF '\t'

# include <iostream>
# include <fstream>
# include <chrono>
# include <algorithm>
# include <cstring>
# include <iomanip>
# include <random>

// for mmmap
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
using namespace std::chrono;



// Node ID above (50 million - 1) will not work
// Used to define an array size of 50 million to keep track of count of each node's neighbors 
#define MAXNODEID 50000000 
#define LINESIZE 100

const char* map_file(char const *fname, size_t& length)
{
   int fd = open(fname, O_RDONLY);
   if (fd == -1){
      std::cout <<"error opening the file" ;
   }
   struct stat sb;
   if(fstat(fd, &sb) == -1)
      std::cout << "error in fstat";
   
   length = sb.st_size;
   std::cout <<"length of the file in map_file is " << length << endl; 
   const char* addr = static_cast<const char*>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));
   if (addr == MAP_FAILED)
      std::cout<< "mmap failed";
   
   return addr;
}


int main( int argc, char const **argv)
{
   if(argc !=8){
   cout << "usage: ./prog <input_graph> <output_graph> <partition_grpah> <rounds> <partitions>";
   }
   std::cout << std::fixed;
   std::cout << std::setprecision(6);
   std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
   start = std::chrono::high_resolution_clock::now();

   
   int *NEIGH_COUNT;
   NEIGH_COUNT = new int[MAXNODEID](); // array of size 50 million for storing neighbor count
   long double *R_COUNT = new long double[MAXNODEID](); // array for storing reciprocal of neighbor count

   char *buffer = new char[LINESIZE](); // max number of letters in each line
   
   long int largest_node_id = 0; // largest node id value
   long int edge_count = 0; // total number of edges
   long int total_unique_nodes = 0; // total number of unique nodes
   int line_number = 0;
   // need to check if length of output file from argv[] is greater than OUTPUT 
   char OUTPUT[64]= "";
   strcpy(OUTPUT, argv[2]); 

   char PARTITION[64] = "";
   strcpy(PARTITION, argv[3]);

   int ROUNDS = atoi(argv[4]);
   int num_partitions = atoi(argv[5]);
   int new_file_flag = atoi(argv[6]);
   char NEW_FILE[64] = "";
   strcpy(NEW_FILE, argv[7]);
   


   size_t length; //size of the file in bytes
   const char *fp = map_file(argv[1], length);
   char letter = '\0'; // letter stores each indicidual letter
   int char_count = 0; //keeps track of the number of characters in each files
   char *token; //tokens from each line using strtok
   long int node1 =0 , node2 = 0;
   char input[100] = "A bird came down the walk";
   // cout << fp;

   // calculating the number of lines
   for (int h = 0 ; h < length; h++){
      letter = fp[h];
      if (letter == '\n'){
         edge_count ++;
      }
   }
   //std::cout<< "number of edges " << edge_count << endl;

   //allocating memory for storing the edge list graph
   int *EDGE_LIST = new int[2* edge_count](); 
   int edge_list_offset = 0;


   //cout << "hello 1" << endl;

   // what if the file is empty ?  then the length is 0
   for (int i = 0 ; i < length; i++){
      letter = fp[i];
      char_count ++; // !!! VERY IMPORTANT TO KEEP TRACK OF NUMBER OF CHARACTERS IN EACH LINE
      //cout << fp[i] << endl;
      // cout << "char_count" << char_count;

      // looks like all input text files have a '\n' after the last line
      if (letter == '\n'){
         //char_count - 1 to remove the '\n at the end'
         for (int j = 0; j < char_count-1 ; j++){
            buffer[j] = fp[i - (char_count - 1) + j];
            }
         
         buffer[char_count - 1] = '\0';
         
         // cout << buffer ;
         char_count = 0; //!!! VERY IMPORTANT REINITIALIZING NUMBER OF CHARACTERS IN EACH LINE

         // parsing each line or edge into two individual nodes using the delimiter '\t'
         char * pch;
         pch = strtok (buffer," \t");
         node1 = atoi(pch);
         //cout << "nodeA " << pch << endl;
         while (pch != NULL)
         {
            //printf ("%s\n",buffer);
            pch = strtok (NULL, " \t");
            if (pch != NULL){
            node2 = atoi(pch);
            //cout << "nodeB " << pch << endl;
            }
         }
         


         // counting the number of neighbors for each node
         NEIGH_COUNT[node1] ++;
         NEIGH_COUNT[node2] ++;

         // finding the largest node_id
         if (node1 > largest_node_id){
            largest_node_id = node1;
         } 
         if(node2 > largest_node_id){
            largest_node_id = node2;
         }
         //cout << "node2" << node2 << endl;

         //storing the edge list in a flat array
         edge_list_offset = 2*line_number;
         EDGE_LIST[edge_list_offset] = node1;
         
         
         EDGE_LIST[edge_list_offset + 1] = node2;
         
         line_number ++; 
         

      }

   }

   // Finding the total number of unique nodes in the edge_list
   //cout << "neighbour count for each node" << endl;
   for (int k =0 ; k <= largest_node_id ; k++){ 
      if (NEIGH_COUNT[k] > 0 ){
         total_unique_nodes++;
      }
   //cout << NEIGH_COUNT[k] << endl;
   }

   //works
   //VERIFICATION : seeing if the egde list is stored in the EDGE_LIST ARRAY
   // if(edge_count == line_number){
   //    cout<< "line number matches edge list"<< endl;
   // }
   // for (int l = 0; l < 15; l++){
   //    std::cout << "n1 "<< EDGE_LIST[2*l] << " n2 "<< EDGE_LIST[2*l + 1] << endl;
   // }

   // storing the reciprocal of the neigh array
   for (int m = 0 ; m <= largest_node_id ; m++){
      if(NEIGH_COUNT[m] >(long double)0){
      R_COUNT[m] = (long double)1/NEIGH_COUNT[m];
      }
      //cout << NEIGH_COUNT[m] << "\t" <<R_COUNT[m] << endl;
   } 
   

   // CREDIT algorithm is working for flicker graph
   // calculating the credit using the EDGE_LIST and R_COUNT array
   // argv[2] is the number of rounds
   // one needs to be replaced with the number of rounds
   // +1 for the 0th index
   long double* CREDIT = new long double[(largest_node_id  * (ROUNDS+1)) + 1 ]();
   int round_offset = 0 ;
   int round_offset_one = 0;
   node1 = 0;
   node2 = 0;
   edge_list_offset = 0;

   for (int n = 1 ; n <=largest_node_id ; n++){
      CREDIT[n] = 1;
   }
   CREDIT[0] = 1;

   //cout << R_COUNT[4] << " r and neigh count  "<< NEIGH_COUNT[4]; 

   int counter = 0 ;
   for (int round = 1 ; round <= ROUNDS ; round++){
      round_offset = round * largest_node_id;
      round_offset_one = (round-1) * largest_node_id;
      //                1   *  10;

      // to fill up credits from the 11th index onwards
      for (int p = 0 ; p <edge_count; p++){
         counter++;
         edge_list_offset = 2*p; 
         node1 = EDGE_LIST[edge_list_offset]; // 0, 2, 4, 6  
         node2 = EDGE_LIST[edge_list_offset +1]; // 1, 3, 5, 7
         //cout << node1 << '\t' << node2 << endl;
         
         // if(node1 == 0 || node2 == 0){
         //    cout << "some node id is 0";
         // }

         // egde = 1 ---- 2
         //credit[r*lnid + 1] = r_count[2] * credit[r*lnid - lnid + 2 ]
         //CREDIT[n1] = r_count[n2] * CREDIT[12 - (round1)*largest_node_id  ]
         CREDIT[round_offset + node1] += R_COUNT[node2] * CREDIT[ round_offset_one + node2];
         CREDIT[round_offset + node2] += R_COUNT[node1] * CREDIT[ round_offset_one + node1];
   }
   }

   // printing all the node ids credit value
   // ROUNDS*largest_node_id
   // for (int r = 1 ; r <= (10); r++){
   //    cout << CREDIT[r] << endl;
   // }
   end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed_seconds_credit = end - start;
   std::cout<< "Elapsed time for calculating credits " << elapsed_seconds_credit.count() << std::endl;

   start = std::chrono::high_resolution_clock::now();
   ofstream o_fd;
   o_fd.open(OUTPUT, ios::out);
   o_fd.precision(6);
   
   for(int s = 1 ; s <= largest_node_id; s++){
      if(NEIGH_COUNT[s] > 0){
      o_fd<< s <<'\t' << NEIGH_COUNT[s];
      for (int t=0; t <= ROUNDS ; t++){
         o_fd<<fixed<<setprecision(6)<<'\t'<<CREDIT[t*largest_node_id + s] ;
      }
      o_fd<<'\n';
      }
   }
   // NOTE: MAKE THIS FLAG ZERO FROM THE COMMAND LINE TO PREVENT CREATING NEW FILES
   if(new_file_flag){
   ofstream p_fd;
   p_fd.open(PARTITION, ios::out);
   // CODE WORKED AFTER CONVERTING VALUES OF PARTITION GRAPH TO STRING
   for (int i = 1; i <= largest_node_id; i++){
      if(NEIGH_COUNT[i] > 0){
         
         p_fd << i << '\t' << NEIGH_COUNT[i] << '\t' << rand() % num_partitions << '\n';
      }
   }
   p_fd.close();

   ofstream new_fd;
   new_fd.open(NEW_FILE, ios::out);
      for (int i = 0 ; i< edge_count; i++){
         new_fd<< EDGE_LIST[2*i] << '\t' << EDGE_LIST[2*i +1] << '\n';
         //cout << EDGE_LIST[2*i] <<'\t' <<EDGE_LIST[2*i+1] <<'\n';
      }
   new_fd.close();
   
   }




   cout << "counter: round * no_of_edges: " << counter << endl;
   std::cout << "largest node id: "<<largest_node_id<< endl;
   std::cout << "neighbors of largest node id: " << NEIGH_COUNT[largest_node_id] << endl;
   std::cout << "total number of lines or edges: " << edge_count << endl; 
   std::cout << "total number of unique nodes: " << total_unique_nodes << endl;

   delete[] NEIGH_COUNT;
   delete[] buffer;
   delete[] CREDIT;
   delete[] R_COUNT;

   end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed_seconds_output = end - start;
   std::cout<< "Elapsed time for writing to file output " << elapsed_seconds_output.count() << std::endl;
   

   return 0;

}

