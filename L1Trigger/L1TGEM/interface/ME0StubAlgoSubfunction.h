#ifndef L1Trigger_L1TGEM_ME0StubAlgoSubfunction_H
#define L1Trigger_L1TGEM_ME0StubAlgoSubfunction_H

#include <cmath>
#include <vector>
#include <map>
#include <cstdint>
#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>
#include "DataFormats/GEMRecHit/interface/ME0StubPrimitive.h"

namespace l1t {
    namespace me0 {
        typedef std::bitset<192> UInt192;

        struct Config {
            bool skip_centroids = false;
            // int ly_thresh = 4;
            std::vector<int> ly_thresh_patid = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 4, 4, 4, 4, 4};
            std::vector<int> ly_thresh_eta = {4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4};
            int max_span = 37;
            int width = 192;
            bool deghost_pre = true;
            bool deghost_post = true;
            int group_width = 8;
            int ghost_width = 1;
            bool x_prt_en = true;
            bool en_non_pointing = false;
            int cross_part_seg_width = 4;
            int num_outputs = 4;
            bool check_ids = false;
            int edge_distance = 2;
            int num_or = 2;
        };

        class hi_lo_t {
        private:
        public:
            int hi, lo;
            hi_lo_t(int hi_, int lo_) : hi(hi_), lo(lo_) {}
        };

        class patdef_t {
        private:
        public:
            int id;
            std::vector<hi_lo_t> layers;
            patdef_t(int id_, std::vector<hi_lo_t> layers_) : id(id_), layers(layers_) {}
        };

        class Mask {
        private:
        public:
            int id;
            std::vector<uint64_t> mask;
            Mask(int id_, std::vector<uint64_t> mask_) : id(id_), mask(mask_) {}
            std::string to_string() const;
        };

        hi_lo_t mirror_hi_lo(const hi_lo_t& ly);
        patdef_t mirror_patdef(const patdef_t& pat, int id);
        std::vector<hi_lo_t> create_pat_ly(double lower, double upper);

        int count_ones(uint64_t x);
        int max_cluster_size(uint64_t x);
        UInt192 set_bit(int index, UInt192 num1);
        UInt192 clear_bit(int index, UInt192 num);
        uint64_t one_bit_mask(int num);
        std::vector<int> find_ones(uint64_t& data);
        std::pair<double,std::vector<int>> find_centroid(uint64_t& data);
        std::vector<std::vector<ME0StubPrimitive>> chunk(const std::vector<ME0StubPrimitive>& in_list, int n);
        void segment_sorter(std::vector<ME0StubPrimitive>& segs, int n);
        std::vector<int> concatVector(const std::vector<std::vector<int>>& vec);
        std::vector<ME0StubPrimitive> concatVector(const std::vector<std::vector<ME0StubPrimitive>>& vec);
    }
}
#endif