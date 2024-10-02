#ifndef DataFormats_ME0Stub_H
#define DataFormats_ME0Stub_H

#include <vector>
#include <cstdint>
#include <string>

class ME0Stub {
private:
    bool ignore_bend = false;
    int lc, hc, id, strip, partition;
    std::vector<float> centroid;
    float substrip = 0.0;
    float bend_ang = 0.0;
    int quality = 0;
public:
    ME0Stub();
    ME0Stub(int lc_,
            int hc_,
            int id_,
            int strip_,
            int partition_);
    ME0Stub(int lc_,
            int hc_,
            int id_,
            int strip_,
            int partition_,
            std::vector<float>& centroid_);
    ME0Stub* clone() const { return new ME0Stub(*this); }
    int LayerCount() const {return lc;}
    int HitCount() const {return hc;}
    int PatternId() const {return id;}
    int Strip() const {return strip;}
    int EtaPartition() const {return partition;}
    float SubStrip() const {return substrip;}
    float BendingAngle() const {return bend_ang;}
    std::vector<float> Centroids() const {return centroid;}
    int Quality() const {return quality;}

    void SetLayerCount(int lc_) {lc=lc_;}
    void SetHitCount(int hc_) {hc=hc_;}
    void SetPatternId(int id_) {id=id_;}
    void SetStrip(int strip_) {strip=strip_;}
    void SetEtaPartition(int partition_) {partition=partition_;}
    void SetCentroids(std::vector<float> centroid_) {centroid=centroid_;}

    void reset();
    void update_quality();
    void fit(int max_span=37);
    // std::string to_string() const;
    bool operator==(const ME0Stub& other);
    bool operator>(const ME0Stub& other);
    bool operator<(const ME0Stub& other);
    bool operator>=(const ME0Stub& other);
    bool operator<=(const ME0Stub& other);
private:
    std::vector<float> llse_fit(const std::vector<float>& x, const std::vector<float>& y);
};

#endif