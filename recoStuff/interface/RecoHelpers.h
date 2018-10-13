#ifndef recoStuff_RecoHelpers_H
#define recoStuff_RecoHelpers_H

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

/** some helper functions that can be applied across
 * different plugins
 */

namespace ff {

  bool genAccept(reco::GenParticle const&);
  
}

#endif