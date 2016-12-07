#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"

//#include "/home/bartok/ParProj/xDRF-src/XDRFExtension/XDRFExtension.cpp"

using namespace llvm;

class xDRFPairData{
  std::vector<std::pair<std::vector<Instruction*>, std::vector<MemoryLocation> > > Pairs;

  bool instInSet(const Instruction* I, std::vector<Instruction*> V){
    std::vector<Instruction*>::iterator iter;
    for(iter = V.begin(); iter != V.end(); iter++){
      if((*iter) == I)
	return true;
    } 
    return false;
  }
  
  MemoryLocation memExtract(const Instruction* I){
    switch (I->getOpcode()){
    case Instruction::Load :
      return MemoryLocation(I->getOperand(0));
      break;
    case Instruction::Store :
      return MemoryLocation(I->getOperand(1));
      break;
    case Instruction::AtomicCmpXchg :
      return MemoryLocation(I->getOperand(0));
      break;
    case Instruction::AtomicRMW :
      return MemoryLocation(I->getOperand(1));
      break;
      
    default :
      return MemoryLocation(NULL);
    }
  }

  bool memLocInSet(const MemoryLocation &Loc, std::vector<MemoryLocation> V){
    std::vector<MemoryLocation>::iterator iter;
    for(iter = V.begin(); iter != V.end(); iter++){
      if((*iter) == Loc)
	return true;
    }
    return false;
  }

  const Instruction* inFromCall(ImmutableCallSite Call){
    return Call.getInstruction();
  }
  
public:
  xDRFPairData(std::vector <std::vector <Instruction*> > In){
    std::vector <std::vector <Instruction*> >::iterator iter_1;
    std::vector<Instruction*>::iterator iter_2;
    
    //Create std::pair(s) of xDRF-regions containing stuffs...
    for(iter_1 = In.begin(); iter_1 != In.end(); iter_1++){
      std::vector<Instruction*> temp_inst;
      std::vector<MemoryLocation> temp_locs;
      for(iter_2 = (*iter_1).begin(); iter_2 != (*iter_1).end(); iter_2++){
	temp_inst.emplace_back(*iter_2);
	MemoryLocation temp = memExtract(*iter_2);
	if(temp.Ptr != NULL)
	  temp_locs.emplace_back(memExtract(*iter_2));
      }
      std::pair<std::vector<Instruction*>, std::vector<MemoryLocation> > temp_pair = std::make_pair(temp_inst,temp_locs);
      Pairs.emplace_back(temp_pair);
    }
  }
  
  bool isPaired(ImmutableCallSite CS, const MemoryLocation &Loc){
    std::vector<std::pair<std::vector<Instruction*>, std::vector<MemoryLocation> > >::iterator iter;
    for(iter = Pairs.begin(); iter != Pairs.end(); iter++){
      if(instInSet(inFromCall(CS),(*iter).first) && memLocInSet(Loc,(*iter).second))
	return true;
    }
    return false;
  }

  bool isPaired(ImmutableCallSite CS1, ImmutableCallSite CS2){
    std::vector<std::pair<std::vector<Instruction*>, std::vector<MemoryLocation> > >::iterator iter;
    for(iter = Pairs.begin(); iter != Pairs.end(); iter++){
      if(instInSet(inFromCall(CS1),(*iter).first) && instInSet(inFromCall(CS2),(*iter).first))
	return true;
    }
    return false;
  }

  bool isPaired(const Instruction *I, ImmutableCallSite Call){
    std::vector<std::pair<std::vector<Instruction*>, std::vector<MemoryLocation> > >::iterator iter;
    for(iter = Pairs.begin(); iter != Pairs.end(); iter++){
      if(instInSet(inFromCall(Call),(*iter).first) && instInSet(I,(*iter).first))
	return true;
    }
    return false;
  }

  bool isPaired(const Instruction *I, const MemoryLocation &Loc){
    std::vector<std::pair<std::vector<Instruction*>, std::vector<MemoryLocation> > >::iterator iter;
    for(iter = Pairs.begin(); iter != Pairs.end(); iter++){
      if(memLocInSet(Loc,(*iter).second) && instInSet(I,(*iter).first))
	return true;
    }
    return false;
  }
};

namespace {
  struct XDRFPair : public ImmutablePass {
    static char ID;
    XDRFPair() : ImmutablePass(ID) {}
  public:

    //xDRFPairData* Data;
    
    xDRFPairData* ref(){
      /*  
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
      */
      return NULL;
    }
    /*
    virtual bool runOnModule(Module &M){
      llvm::dbgs() << "Executed XDRFPair!\n";
      return false;
    }
    */
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      //AU.addRequired<XDRFExtension>();
    }
    
  };
}

char XDRFPair::ID = 0;
static RegisterPass<XDRFPair> G("XDRFPair", "Extented xDRF-Region analysis", false, true);

