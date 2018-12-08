#include "Firefighter/ffNtuple/interface/ffNtupleBase.h"

class ffNtupleEvent : public ffNtupleBase
{
  public:
    ffNtupleEvent(const edm::ParameterSet&);

    void initialize(TTree&, const edm::ParameterSet&, edm::ConsumesCollector&&) final;
    void fill(const edm::Event&, const edm::EventSetup&) final;
    void fill(const edm::Event&, const edm::EventSetup&, HLTConfigProvider&) override {}

  private:
    void clear() final;

    int run_;
    int event_;
    int lumi_;
};

DEFINE_EDM_PLUGIN(ffNtupleFactory,
                  ffNtupleEvent,
                  "ffNtupleEvent");


ffNtupleEvent::ffNtupleEvent(const edm::ParameterSet& ps) :
  ffNtupleBase(ps)
{
}

void
ffNtupleEvent::initialize(TTree& tree,
                          const edm::ParameterSet& ps,
                          edm::ConsumesCollector&& cc)
{
  clear();

  tree.Branch("run",   &run_,   "run/I");
  tree.Branch("event", &event_, "event/I");
  tree.Branch("lumi",  &lumi_,  "lumi/I");
}

void
ffNtupleEvent::fill(const edm::Event& e,
                    const edm::EventSetup& es)
{
  run_   = e.id().run();
  event_ = e.id().event();
  lumi_  = e.luminosityBlock();
}

void
ffNtupleEvent::clear()
{
  run_   = 0;
  event_ = 0;
  lumi_  = 0;
}