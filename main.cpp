/**
   Program: chunking based deduplicator/reduplicator
   Author: Victor Brunko
   Nov 28, 2017

   Generic algorithm:
   1. Read chunk-by-chunk the input file
   2. Calculate the hash sum of each chunk
   3. Map realtion: chunkX - {count++:position}
   4. Export MAP to output file including DATA

   Output format example for the deduped file:
   **************** FILE_OUT BEGIN ****************
   1
   5
   ddddd
   50
   1
   10
   cccccccccc
   30
   2
   10
   aaaaaaaaaa
   0
   10
   2
   10
   bbbbbbbbbb
   20
   40
   **************** FILE_OUT END ****************

   where (last five records from FILE_OUT explained):
   2  // How many chunks
   10  // Chunk's size
   bbbbbbbbbb // binary data
   20 // position #1
   40 // position #2

 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

using namespace std;

const int CHUNK_SIZE = 10;

// Information about the actual chunk:
struct pos_and_size {
  size_t pos;
  size_t size;
};

typedef std::map<size_t, list<pos_and_size>> dupmap;

uint32_t hash_block(vector<char> chunk) {
  uint32_t hash = 0;

  for (const auto c : chunk) {
    hash += c;
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}

// Export file helper function
void export_file(dupmap file_map, ifstream &file_in, ofstream &file_out) {

  char data[CHUNK_SIZE];
  int l_chunk_pos = 0;
  int l_chunk_size = 0;

  // Iterate through file map:
  for (const auto it : file_map) {
    list<pos_and_size> l;
    l = it.second;

    bool once = false;

    for (const auto itl : l) {
      l_chunk_pos = itl.pos;
      l_chunk_size = itl.size;
      if (!once) {
        once = true;
        file_out << l.size() << endl;     // How many chunks
        file_out << l_chunk_size << endl; // Current chunk size
        file_in.seekg(l_chunk_pos, file_in.beg);
        file_in.read(data, l_chunk_size); // Save the actual chunk:
        file_out.write(data, l_chunk_size);
      }
      file_out << l_chunk_pos << endl; // Positions located
    }
  }
}

void dedup(std::string inpath, std::string outpath) {

  ifstream file_in(inpath, ifstream::binary);
  ofstream file_out(outpath, ifstream::binary);

  if (!file_in) {
    cerr << "error while opening file_in: " << inpath << endl;
    exit(1);
  }

  if (!file_out) {
    cerr << "error while opening file_out: " << inpath << endl;
    exit(1);
  }

  file_in.seekg(0, file_in.end);

  dupmap file_map; // hash:count
  size_t total_size = file_in.tellg();
  size_t chunk_size = CHUNK_SIZE;
  size_t total_chunks = total_size / chunk_size;
  size_t last_chunk_size = total_size % chunk_size;
  size_t hash = 0;

  if (last_chunk_size != 0)
    ++total_chunks;
  else
    last_chunk_size = chunk_size;

  /* the loop of chunking */
  file_in.seekg(0, file_in.beg);
  for (size_t chunk = 0; chunk < total_chunks; ++chunk) {

    size_t this_chunk_size =
        chunk == total_chunks - 1 /* if last chunk */
            ? last_chunk_size     /* then fill chunk with remaining bytes */
            : chunk_size;         /* else fill entire chunk */
    size_t start_of_chunk = chunk * chunk_size;
    list<pos_and_size> l;
    pos_and_size pands;
    vector<char> chunk_data(this_chunk_size);

    file_in.read(&chunk_data[0], this_chunk_size);

    hash = hash_block(chunk_data);
    l = file_map[hash];

    pands.pos = start_of_chunk;
    pands.size = this_chunk_size;

    l.push_back(pands);
    file_map[hash] = l;
  }

  export_file(file_map, file_in, file_out);
}

bool redup(std::string inpath, std::string outpath) {

  ifstream file_in(inpath, ifstream::binary);
  ofstream file_out(outpath, ifstream::binary);

  if (!file_in) {
    cerr << "error while opening file_in: " << inpath << endl;
    exit(1);
  }

  if (!file_out) {
    cerr << "error while opening file_out: " << inpath << endl;
    exit(1);
  }

  char line[64];
  char data[CHUNK_SIZE];
  size_t count = 0;
  size_t pos = 0;
  size_t size = 0;

  while (file_in) {
    file_in.getline(line, 64);
    count = atof(line);
    if (count <= 0) {
      break;
    }
    file_in.getline(line, 64);
    size = atoi(line);
    memset(data, 0, 64);
    file_in.read(data, size);

    while (count--) {
      file_in.getline(line, 64);
      pos = atoi(line);

      file_out.seekp(pos, file_in.beg);
      file_out.write(data, size);
    }
  }

  return true;
}

int main() {
  dedup("./file.in", "./file.out");
  redup("./file.out", "./file.re");
}
