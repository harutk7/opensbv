//
// Created by harut on 9/5/18.
//

#ifndef OPENSBV_CHUNKSPLITTER_H
#define OPENSBV_CHUNKSPLITTER_H

#include <iostream>
#include <vector>
#include <list>

namespace opensbv {
    namespace streamer {

        class ChunkSplitter {

            std::list<std::vector<unsigned char>> mList;

            const size_t mChunkSize;

        public:
            ChunkSplitter(size_t chunkSize);
            ~ChunkSplitter();

            void split(unsigned char *buf, size_t n, long long int timeStamp);

            bool hasNext();

            std::vector<unsigned char> getNext();

            void deleteNext();
        };
    }
}


#endif //OPENSBV_CHUNKSPLITTER_H
