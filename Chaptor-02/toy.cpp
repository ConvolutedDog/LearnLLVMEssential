// Compile: clang++ -O3 toy.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o toy
// Run: ./toy

#include <iostream>

#include "llvm/IR/LLVMContext.h" // llvm::LLVMContext
static llvm::LLVMContext Context;

#include "llvm/IR/Module.h" // llvm::Module
static llvm::Module *ModuleOb = new llvm::Module("toy.cpp", Context);

#include <vector>
#include <string>
static std::vector<std::string> FuncArgs;

typedef llvm::SmallVector<llvm::BasicBlock*, 16> BBList;
typedef llvm::SmallVector<llvm::Value*, 16> ValList;

#include "llvm/IR/Function.h" // llvm::Function
#include "llvm/IR/IRBuilder.h" // llvm::IRBuilder
#include "llvm/IR/DerivedTypes.h" // llvm::FunctionType
llvm::Function *createFunc(llvm::IRBuilder<> &Builder, std::string Name) {
  std::vector<llvm::Type *> Integers(FuncArgs.size(), llvm::Type::getInt32Ty(Context));
  llvm::FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), Integers, false);
  llvm::Function *fooFunc = llvm::Function::Create(
    funcType, llvm::Function::ExternalLinkage, Name, ModuleOb
  );
  return fooFunc;
}

void setFuncArgs(llvm::Function *fooFunc, std::vector<std::string> FuncArgs) {
  unsigned Idx = 0;
  llvm::Function::arg_iterator AI, AE;
  for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE; ++AI, ++Idx)
    AI->setName(FuncArgs[Idx]);
}

llvm::BasicBlock *createBB(llvm::Function *fooFunc, std::string Name) {
  return llvm::BasicBlock::Create(Context, Name, fooFunc);
}

#include "llvm/IR/GlobalVariable.h" // llvm::GlobalVariable
#include "llvm/Support/Alignment.h" // llvm::MaybeAlign
llvm::GlobalVariable *createGlob(llvm::IRBuilder<> &Builder, std::string Name) {
  ModuleOb->getOrInsertGlobal(Name, Builder.getInt32Ty());
  llvm::GlobalVariable *gVar = ModuleOb->getNamedGlobal(Name);
  gVar->setLinkage(llvm::GlobalVariable::CommonLinkage);
  gVar->setAlignment(llvm::MaybeAlign(4));
  return gVar;
}

#include "llvm/IR/Value.h" // llvm::Value
llvm::Value *createArithMul(llvm::IRBuilder<> &Builder, llvm::Value *Lhs, llvm::Value *Rhs) {
  return Builder.CreateMul(Lhs, Rhs, "multmp");
}
llvm::Value *createArithAdd(llvm::IRBuilder<> &Builder, llvm::Value *Lhs, llvm::Value *Rhs) {
  return Builder.CreateAdd(Lhs, Rhs, "addtmp");
}

llvm::Value *createIfElse(llvm::IRBuilder<> &Builder, ValList &VL, BBList &BBL) {
  llvm::Value *Condtn = VL[0];
  llvm::Value *Arg1 = VL[1];
  llvm::Value *Arg2 = VL[2];
  llvm::BasicBlock *ThenBB = BBL[0];
  llvm::BasicBlock *ElseBB = BBL[1];
  llvm::BasicBlock *MergeBB = BBL[2];
  Builder.CreateCondBr(Condtn, ThenBB, ElseBB);

  Builder.SetInsertPoint(ThenBB);
  llvm::Value *ThenVal = Builder.CreateAdd(Arg1, Builder.getInt32(1), "thenaddtmp");
  Builder.CreateBr(MergeBB);

  Builder.SetInsertPoint(ElseBB);
  llvm::Value *ElseVal = Builder.CreateAdd(Arg2, Builder.getInt32(1), "elseaddtmp");
  Builder.CreateBr(MergeBB);

  unsigned PhiBBSize = BBL.size() - 1;
  Builder.SetInsertPoint(MergeBB);
  llvm::PHINode *Phi = Builder.CreatePHI(llvm::Type::getInt32Ty(Context), PhiBBSize, "iftmp");

  Phi->addIncoming(ThenVal, ThenBB);
  Phi->addIncoming(ElseVal, ElseBB);

  return Phi;
}

llvm::Value *createLoop(llvm::IRBuilder<> &Builder, BBList &BBL, ValList VL,
                        llvm::Value *StartVal, llvm::Value *EndVal) {
  llvm::Value *Arg1 = VL[1];
  llvm::Value *Arg2 = VL[2];
  llvm::BasicBlock *LoopBB = BBL[0];
  llvm::BasicBlock *AfterBB = BBL[1];

  llvm::BasicBlock *PreHeaderBB = Builder.GetInsertBlock();

  Builder.CreateBr(LoopBB);
  Builder.SetInsertPoint(LoopBB);

  llvm::PHINode *IndVar = Builder.CreatePHI(llvm::Type::getInt32Ty(Context), 2, "i");
  IndVar->addIncoming(StartVal, PreHeaderBB);

  llvm::Value *RetVal = Builder.CreateAdd(Arg1, Builder.getInt32(5), "addtmp");
  llvm::Value *StepVal = Builder.getInt32(1);
  llvm::Value *NextVal = Builder.CreateAdd(IndVar, StepVal, "nextval");
  llvm::Value *EndCond = Builder.CreateICmpNE(IndVar, EndVal, "endcond");
  EndCond = Builder.CreateICmpNE(EndCond, Builder.getFalse(), "loopcond");

  llvm::BasicBlock *LoopEndBB = Builder.GetInsertBlock();
  Builder.CreateCondBr(EndCond, LoopBB, AfterBB);

  Builder.SetInsertPoint(AfterBB);
  IndVar->addIncoming(NextVal, LoopEndBB);

  return RetVal;
}

#include "llvm/IR/Verifier.h" // llvm::verifyFunction
int main(int argc, char **argv) {
  std::cout << "============= Empty llvm::Module =============" << std::endl;

  ModuleOb->dump();

  std::cout << "============= Create llvm::GlobalVariable =============" << std::endl;

  static llvm::IRBuilder<> Builder(Context);
  llvm::GlobalVariable *gVar = createGlob(Builder, "x");
  ModuleOb->dump();

  std::cout << "============= setFuncArgs and Create llvm::Function =============" << std::endl;

  FuncArgs.push_back("a");
  FuncArgs.push_back("b");
  llvm::Function *fooFunc = createFunc(Builder, "foo");
  setFuncArgs(fooFunc, FuncArgs);
  llvm::verifyFunction(*fooFunc);
  ModuleOb->dump();

  std::cout << "============= Create llvm::BasicBlock =============" << std::endl;

  llvm::BasicBlock *entry = createBB(fooFunc, "entry");
  Builder.SetInsertPoint(entry);
  llvm::verifyFunction(*fooFunc);
  ModuleOb->dump();

  std::cout << "============= CreateMul or other Arith =============" << std::endl;

  llvm::Value *constant1 = Builder.getInt32(16);
  llvm::Value *Arg1 = fooFunc->arg_begin();
  llvm::Value *Arg2 = &*std::next(fooFunc->arg_begin());
  llvm::Value *val1 = createArithMul(Builder, Arg1, constant1);
  llvm::Value *val2 = createArithAdd(Builder, Arg2, val1);
  llvm::verifyFunction(*fooFunc);
  ModuleOb->dump();

  std::cout << "============= If-Else-Statement =============" << std::endl;
  std::cout << val1->getType() << " " << val2->getType() << std::endl;
  llvm::Value *Compare = Builder.CreateICmpULT(val1, val2, "cmptmp");
  std::cout << Compare->getType() << " " << Builder.getInt32(0)->getType() << std::endl;
  llvm::Value *Condtn = Builder.CreateICmpNE(Compare, Builder.getFalse(), "ifcond");

  ValList VL;
  VL.push_back(Condtn);
  VL.push_back(Arg1);
  VL.push_back(Arg2);

  llvm::BasicBlock *ThenBB = createBB(fooFunc, "ThenBB");
  llvm::BasicBlock *ElseBB = createBB(fooFunc, "ElseBB");
  llvm::BasicBlock *MergeBB = createBB(fooFunc, "MergeBB");

  BBList BBL1;
  BBL1.push_back(ThenBB);
  BBL1.push_back(ElseBB);
  BBL1.push_back(MergeBB);

  llvm::Value *v = createIfElse(Builder, VL, BBL1);
  
  llvm::verifyFunction(*fooFunc);
  ModuleOb->dump();

  std::cout << "============= For-Loop-Statement =============" << std::endl;

  llvm::BasicBlock *LoopBB = createBB(fooFunc, "LoopBB");
  llvm::BasicBlock *AfterBB = createBB(fooFunc, "AfterBB");

  BBList BBL2;
  BBL2.push_back(LoopBB);
  BBL2.push_back(AfterBB);

  llvm::Value *StartVal = Builder.getInt32(1);
  llvm::Value *RetVal = createLoop(Builder, BBL2, VL, StartVal, Arg2);
  
  llvm::verifyFunction(*fooFunc);
  ModuleOb->dump();

  std::cout << "============= CreateRet =============" << std::endl;

  // Builder.CreateRet(Builder.getInt32(0));
  // Builder.CreateRet(val2);
  // Builder.CreateRet(v);
  Builder.CreateRet(RetVal);
  llvm::verifyFunction(*fooFunc);
  ModuleOb->dump();

  return 0;
}
