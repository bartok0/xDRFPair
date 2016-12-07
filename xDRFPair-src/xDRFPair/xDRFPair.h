#ifndef _XDRFPair
#define _XDRFPair

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "/home/bartok/ParProj/xDRF-src/XDRFExtension/XDRFExtension.cpp"

class xDRFPairData{
private:
  std::vector<std::pair<std::vector<Instruction*>, std::vector<MemoryLocation> > > Pairs;
  
   bool instInSet(const Instruction* I, std::vector<Instruction*> V);
  MemoryLocation memExtract(const Instruction* I);
  bool memLocInSet(const MemoryLocation &Loc, std::vector<MemoryLocation> V);
  const Instruction* inFromCall(ImmutableCallSite Call);
  
public:
  xDRFPairData( ) { }
  xDRFPairData(std::vector <std::vector <Instruction*> > In){
    std::vector <std::vector <Instruction*> >::iterator iter_1;
    std::vector<Instruction*>::iterator iter_2;
    
    //Create std::pair(s) of xDRF-regions containing stuffs...
    for(iter_1 = In.begin(); iter_1 != In.end(); iter_1++){
      std::vector<Instruction*> temp_inst;
      std::vector<MemoryLocation> temp_locs;
      for(iter_2 = (*iter_1).begin(); iter_2 != (*iter_1).end(); iter_2++){
	temp_inst.emplace_back(*iter_2);
	temp_locs.emplace_back(memExtract(*iter_2));
      }
      std::pair<std::vector<Instruction*>, std::vector<MemoryLocation> > temp_pair = std::make_pair(temp_inst,temp_locs);
      Pairs.emplace_back(temp_pair);
    }
   }
  
  bool isPaired(ImmutableCallSite CS1, ImmutableCallSite CS2);
  bool isPaired(ImmutableCallSite CS, const MemoryLocation &Loc);
  bool isPaired(const Instruction *I, ImmutableCallSite Call);
  bool isPaired(const Instruction *I, const MemoryLocation &Loc);
  
};

namespace {
  struct XDRFPair : public ModulePass {
    static char ID;
    XDRFPair() : ModulePass(ID) {}
    
      //xDRFPairData* Data;
    
    xDRFPairData* ref(){
      XDRFExtension& XDRFS = getAnalysis<XDRFExtension>();
      std::vector<std::vector<Instruction*> > vect;
      SmallPtrSet<xDRFRegion*,6> xDRFRegs = XDRFS.xDRFRegions;
      SmallPtrSet<xDRFRegion*,6>::iterator xdrf_iter = xDRFRegs.begin();
      
      for(; xdrf_iter != xDRFRegs.end(); xdrf_iter++){
	std::vector<Instruction*> temp;
	SmallPtrSet<Instruction*,128> curr = (*xdrf_iter)->getAssociatedInstructions();
	
	SmallPtrSet<Instruction*,128>::iterator inst_iter = curr.begin();
	for(; inst_iter != curr.end(); inst_iter++){
	  temp.emplace_back(*inst_iter);
	}
	vect.emplace_back(temp);
      }
      
      xDRFPairData D = xDRFPairData(vect);
      xDRFPairData* D_ref = &D;
      return D_ref;
    }
    
    bool runOnModule(Module &M){
      return true;
    }
      
    void getAnalysisUsage(AnalysisUsage &AU){
      AU.addRequired<XDRFExtension>();
    }
    
  };
}

char XDRFPair::ID = 0;
static RegisterPass<XDRFPair> G("XDRFPair", "Extented xDRF-Region analysis", false, false);

#endif
