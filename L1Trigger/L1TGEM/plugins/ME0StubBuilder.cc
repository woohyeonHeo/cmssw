#include "L1Trigger/L1TGEM/plugins/ME0StubBuilder.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigi.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartition.h"
#include "L1Trigger/L1TGEM/interface/ME0StubAlgoChamber.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace l1t::me0;

typedef std::vector<std::vector<UInt192>> ME0ChamberData;
typedef std::vector<std::vector<std::vector<int>>> ME0ChamberBXData;

ME0StubBuilder::ME0StubBuilder(const edm::ParameterSet& ps) {
    PileUp = ps.getParameter<int32_t>("PileUp");
}
ME0StubBuilder::~ME0StubBuilder() {}

void ME0StubBuilder::fillDescription(edm::ParameterSetDescription& desc) {
    desc.add<int32_t>("PileUp", 0);
}

void ME0StubBuilder::build(const GEMPadDigiCollection* paddigis, ME0StubCollection& oc) {
    // edm::LogVerbatim("ME0StubBuilder") << "[ME0StubBuilder::build] Total number of rechits in this event: "
    //                                         << digis->size();
    
    // Let's define the ensemble of GEM devices having the same region, chambers number (phi)
    // different eta partitions and different layers are allowed

    Config config;
    config.num_outputs=10;
    config.cross_part_seg_width=4;
    config.ghost_width=10;
    if (PileUp==200) {
        config.ly_thresh_patid = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 4};
        config.ly_thresh_eta = {4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 5};
    }

    std::map<uint32_t, std::pair<ME0ChamberData,ME0ChamberBXData>> DataMap;
    // Loop on the GEM rechit and select the different GEM Ensemble
    for (auto it = paddigis->begin(); it != paddigis->end(); ++it) {
        GEMDetId gemid((*it).first);
        if (gemid.station() != 0) continue;

        uint32_t gemRawId = (gemid.superChamberId()).rawId();

        if (DataMap[gemRawId].first.empty() || DataMap[gemRawId].second.empty()) {
            DataMap[gemRawId].first
                = std::vector<std::vector<UInt192>>(8,std::vector<UInt192>(6,UInt192(0)));
            DataMap[gemRawId].second
                = std::vector<std::vector<std::vector<int>>>(8,std::vector<std::vector<int>>(6,std::vector<int>(192,-9999)));
        }
        int layer = gemid.layer();
        int ieta = gemid.ieta();
        for (auto paddigi = ((*it).second).first; paddigi != ((*it).second).second; ++paddigi) {
            int strip = (*paddigi).pad();
            (DataMap[gemRawId].first.at(ieta-1)).at(layer-1) |= (UInt192(1) << (strip));
            ((DataMap[gemRawId].second.at(ieta-1)).at(layer-1)).at(strip) = (*paddigi).bx();
        }
    }

    // Find stub per chamber using DataMap
    for (const auto& data_pair : DataMap) {
        uint32_t rawId = data_pair.first;
        auto data = data_pair.second.first;
        auto bx_data = data_pair.second.second;

        bool isNoneZero = false;
        for (const auto& etaData : data) {
            for (const auto& ly : etaData) {
                if (ly.any()) {
                    isNoneZero = true;
                    break;
                }
            }
            if (isNoneZero) break;
        }
        if (!isNoneZero) continue;

        GEMDetId id(rawId);

        std::vector<ME0StubPrimitive> SegList = process_chamber(data, bx_data, config);

        std::vector<ME0Stub> SegList_processed;
        
        double mse_thresh = 0.75;
        for (ME0StubPrimitive& seg : SegList) {
            seg.fit(config.max_span);
            if (seg.MSE() >= mse_thresh) {
                seg.reset();
            }
            if (PileUp==200) {
                if (std::abs(seg.BendingAngle()) > 1) seg.reset();
                if (seg.EtaPartition() >= 9) {
                    if (std::abs(seg.BendingAngle()) > 0.5) seg.reset();
                }
            }

            if (seg.PatternId() == 0) continue;
            if ((seg.EtaPartition() % 2) != 0) seg.SetEtaPartition(seg.EtaPartition()/2 + 1);
            else seg.SetEtaPartition(seg.EtaPartition()/2);

            ME0Stub seg_final(id, seg);

            SegList_processed.push_back(seg_final);
        }

        for (auto seg : SegList_processed) {
            std::cout<<"region="<<id.region()<<", chamber="<<id.chamber()
                      <<", "<<seg<<std::endl;
        }

        oc.put(id, SegList_processed.begin(), SegList_processed.end());
    }
}