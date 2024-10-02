#include "DataFormats/GEMRecHit/interface/ME0Stub.h"

//define class ME0Stub
ME0Stub::ME0Stub() : lc{0}, hc{0}, id{0}, strip{0}, partition{0} {
    update_quality();
}
ME0Stub::ME0Stub(int lc_,
                 int hc_,
                 int id_,
                 int strip_,
                 int partition_) :
                lc{lc_}, hc{hc_}, id{id_}, strip{strip_}, partition{partition_} {
    update_quality();
}
ME0Stub::ME0Stub(int lc_,
                 int hc_,
                 int id_,
                 int strip_,
                 int partition_,
                 std::vector<float>& centroid_) :
                lc{lc_}, hc{hc_}, id{id_}, strip{strip_}, partition{partition_}, centroid{centroid_} {
    update_quality();
}
void ME0Stub::reset() {
    lc = 0; hc = 0; id = 0;
    update_quality();
}
void ME0Stub::update_quality() {
    int idmask;
    if (lc) {
        if (ignore_bend) {idmask = 0xfe;}
        else {idmask = 0xff;}
        quality = (lc << 23) | (hc << 17) | ((id & idmask) << 12) | (strip << 4) | partition;
    } else {quality = 0;}
}
void ME0Stub::fit(int max_span) {
    if (id) {
        std::vector<float> tmp;
        for (float cent : centroid) {
            tmp.push_back(cent-(max_span/2+1));
        }
        std::vector<float> x;
        std::vector<float> centroids;
        for (uint32_t i=0; i < tmp.size(); ++i) {
            if (tmp[i] != -1*(max_span/2+1)) {
                x.push_back(i-2.5);
                centroids.push_back(tmp[i]);
            }
        }
        std::vector<float> fit = llse_fit(x, centroids);
        bend_ang = fit[0];
        substrip = fit[1];
    }
}
std::vector<float> ME0Stub::llse_fit(const std::vector<float>& x, const std::vector<float>& y) {
    float x_sum = 0;
    float y_sum = 0;
    for (float val : x) {x_sum += val;}
    for (float val : y) {y_sum += val;}
    int n = x.size();
    // linear regression
    float product = 0;
    float squares = 0;
    for (int i=0; i<n; ++i) {
        product += (n*x[i] - x_sum)*(n*y[i] - y_sum);
        squares += (n*x[i] - x_sum)*(n*x[i] - x_sum);
    }

    float m = product/squares;
    float b = (y_sum - m*x_sum)/n;
    std::vector<float> fit = {m, b};
    return fit;
}
// std::string ME0Stub::to_string() const {}
bool ME0Stub::operator==(const ME0Stub& other) {
    if (lc == 0 && other.lc == 0) {return true;}
    return (quality == other.quality);
}
bool ME0Stub::operator>(const ME0Stub& other) {
    return (quality > other.quality);
}
bool ME0Stub::operator<(const ME0Stub& other) {
    return (quality < other.quality);
}
bool ME0Stub::operator>=(const ME0Stub& other) {
    return (quality >= other.quality);
}
bool ME0Stub::operator<=(const ME0Stub& other) {
    return (quality <= other.quality);
}
