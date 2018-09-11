//
// Created by harut on 7/23/18.
//

#include <algorithm>
#include "opensbv/streamer/captureMRTP.h"

namespace opensbv {
    namespace streamer {

        // MAIN CLASS
        CaptureMRTP::CaptureMRTP(): mCombiner(), mLength(0), mData(nullptr) {
        }

        CaptureMRTP::~CaptureMRTP() {
            delete mData;
        }

        void CaptureMRTP::onRecv(char *buf, size_t n) {

            mCombiner.add(buf, n);

            if (mCombiner.isReady())
                fill(mCombiner.getNext());
        }

        std::vector<unsigned char> CaptureMRTP::getData() {
            std::vector<unsigned char> vec = {};

            if (mData == nullptr)
                return vec;

            mTx.lock();
            std::copy(mData + 0, mData + mLength, std::back_inserter(vec));
            delete mData;
            mData = nullptr;
            mTx.unlock();
            return vec;
        }

        long long int CaptureMRTP::getTimestamp() {
            return mCombiner.getTimestmap();
        }

        void CaptureMRTP::fill(std::vector<unsigned char> &vec) {
            mTx.lock();
            delete mData;
            mLength = vec.size();
            mData = new unsigned char[vec.size()];
            std::copy(vec.begin(), vec.end(), mData + 0);
            mTx.unlock();
        }

        // CHUNK COMBINER CLASS
        ChunkCombiner::ChunkCombiner() {

        }

        ChunkCombiner::~ChunkCombiner() {

        }

        bool ChunkCombiner::isReady() {
            return mReady;
        }

        std::vector<unsigned char>& ChunkCombiner::getNext() {

            return mData;
        }

        void ChunkCombiner::deleteNext() {

        }

        long long int ChunkCombiner::getTimestmap() {
            return mTimestamp;
        }

        void ChunkCombiner::add(char *buf, size_t n) {

            m_remList.clear();

            std::string bufStr(buf + (n - 30), buf + n);

            std::size_t found = bufStr.find("-+-");
            if (found!=std::string::npos) {
                std::string str2 = bufStr.substr (found + 3,bufStr.length() - 1);

                auto infos = opensbv::helpers::general::GeneralHelper::explode(str2, '-');

                if (infos.size() == 4) {
                    size_t length = std::stoi(infos[0]);
                    int maxChunk = std::stoi(infos[1]);
                    int curChunk = std::stoi(infos[2]);
                    long long int timestamp = std::stoul(infos[3]);

                    if (curChunk == 1) {
                        m_units.emplace_back(ChunkUnit(maxChunk, timestamp));

                    }
                    for (int i = 0; i <m_units.size() ; i++) {

                        if(m_units[i].m_timestamp == timestamp) {
                            m_units[i].m_curchunk++;
                            m_units[i].fill(buf, (n - (str2.length() + 3)));
                        }

                    }

                    for (int i = 0; i <m_units.size() ; ++i) {
                        if(m_units[i].m_curchunk == m_units[i].m_maxchunk && m_units[i].m_timestamp>= mTimestamp) {
                            mTimestamp = m_units[i].m_timestamp;
                            mReady = true;
                            mData = std::vector<unsigned char> (m_units[i].m_data);
                        }

                        if (timestamp - m_units[i].m_timestamp > 1000) {
                            m_remList.emplace_back(i);
                        }
                    }

                    for (int i = 0; i <m_units.size() ; ++i) {

                        if(m_units[i].m_curchunk == m_units[i].m_maxchunk && m_units[i].m_timestamp>= mTimestamp) {
                            mTimestamp = m_units[i].m_timestamp;
                            mReady = true;
                            mData = std::vector<unsigned char> (m_units[i].m_data);
                        }
                    }
                    for(int item : m_remList) {
                        m_units.erase(m_units.begin() + item);
                    }

//                    vec.erase(vec.begin() + 1);
//                        m_units(std::remove_if(m_list.begin(), m_list.end(), [](const KCFObject & o) {
////            cv::Mat1b mask1;
////            inRange(hsv, cv::Scalar(255, 255, 255), cv::Scalar(255, 255, 255), mask1);
//                            bool lost = o.m_status == OBJ_STATUS_LOST;
//                            bool is_inside = (o.m_box & cv::Rect2d(0, 0, 1280, 720)) == o.m_box;
//                            return !is_inside || lost;
//                        }), m_units.end());
//                    mLength += (n - (str2.length() + 3));
//                    std::copy(buf + 0, buf + (n - (str2.length() + 3)), std::back_inserter(mData));
//                    mReady = mLength == length;
                }
            }

        }

    }
}