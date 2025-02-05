#ifndef L1Trigger_L1TGEM_ME0StubBuilder_h
#define L1Trigger_L1TGEM_ME0StubBuilder_h

/** \class ME0StubBuilder derived by GEMSegmentBuilder
 * Algorithm to build ME0Stub's from GEMPadDigi collection
 * by implementing a 'build' function required by ME0StubProducer.
 *
 * Implementation notes: <BR>
 * Configured via the Producer's ParameterSet. <BR>
 * Presume this might become an abstract base class one day. <BR>
 *
 * \author Woohyeon Heo
 *
*/
#include <cstdint>

#include "DataFormats/GEMDigi/interface/GEMPadDigiCollection.h"
#include "DataFormats/GEMRecHit/interface/ME0StubPrimitive.h"
#include "DataFormats/GEMRecHit/interface/ME0StubCollection.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

// using namespace l1t::me0;

class ME0StubAlgorithmBase;

class ME0StubBuilder {
public:
  /** Configure the algorithm via ctor.
     * Receives ParameterSet percolated down from EDProducer
     * which owns this Builder.
     */
  explicit ME0StubBuilder(const edm::ParameterSet&);
  /// Destructor
  ~ME0StubBuilder();

  /** Find stubs in each ensemble of 6 GEM layers, build ME0Stub's ,
     *  and fill into output collection.
     */
  void build(const GEMPadDigiCollection* paddigis, ME0StubCollection& oc);

  /** Cache pointer to geometry _for current event_
     */

  static void fillDescription(edm::ParameterSetDescription& descriptions);

private:
   int32_t PileUp;
};

#endif
