// Compile: clang++ -O3 toy.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o toy
// Run: ./toy

#include <iostream>

#include "llvm/IR/LLVMContext.h" // llvm::LLVMContext
static llvm::LLVMContext Context1;
static llvm::LLVMContext Context2;

#include "llvm/IR/Module.h" // llvm::Module
static llvm::Module *ModuleOb1 = new llvm::Module("toy.cpp", Context1);
static llvm::Module *ModuleOb2 = new llvm::Module("toy.cpp", Context2);

#include <vector>
#include <string>
static std::vector<std::string> FuncArgs1;
static std::vector<std::string> FuncArgs2;

#define vectorSize 16

#include "llvm/IR/IRBuilder.h" // llvm::IRBuilder
#include "llvm/IR/DerivedTypes.h" // llvm::VectorType
llvm::Function *createFunc1(llvm::IRBuilder<> &Builder, std::string Name) {
  llvm::Type *u32Ty = llvm::Type::getInt32Ty(Context1);
  llvm::VectorType *vecTy = llvm::VectorType::get(u32Ty, vectorSize, false);
  llvm::Type *ptrTy = vecTy->getPointerTo(0);
  llvm::FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), ptrTy, false);
  llvm::Function *fooFunc = llvm::Function::Create(
    funcType, llvm::Function::ExternalLinkage, Name, ModuleOb1
  );
  return fooFunc;
}

llvm::Function *createFunc2(llvm::IRBuilder<> &Builder, std::string Name) {
  llvm::Type *u32Ty = llvm::Type::getInt32Ty(Context2);
  llvm::VectorType *vecTy = llvm::VectorType::get(u32Ty, vectorSize, true);
  llvm::FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), vecTy, false);
  llvm::Function *fooFunc = llvm::Function::Create(
    funcType, llvm::Function::ExternalLinkage, Name, ModuleOb2
  );
  return fooFunc;
}

void setFuncArgs(llvm::Function *fooFunc, std::vector<std::string> FuncArgs1) {
  unsigned Idx = 0;
  llvm::Function::arg_iterator AI, AE;
  for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE; ++AI, ++Idx)
    AI->setName(FuncArgs1[Idx]);
}

llvm::BasicBlock *createBB1(llvm::Function *fooFunc, std::string Name) {
  return llvm::BasicBlock::Create(Context1, Name, fooFunc);
}

llvm::BasicBlock *createBB2(llvm::Function *fooFunc, std::string Name) {
  return llvm::BasicBlock::Create(Context2, Name, fooFunc);
}

llvm::Value *getGEP(llvm::IRBuilder<> &Builder, llvm::Value *Base, llvm::Value *Offset) {
  return Builder.CreateGEP(Builder.getInt32Ty(), Base, Offset, "a1");
}

llvm::Value *getLoad(llvm::IRBuilder<> &Builder, llvm::Value *Address) {
  return Builder.CreateLoad(Builder.getInt32Ty(), Address, "load");
}

void getStore(llvm::IRBuilder<> &Builder, llvm::Value *Address, llvm::Value *V) {
  Builder.CreateStore(V, Address, false);
}

#include "llvm/IR/Value.h" // llvm::Value
llvm::Value *createArithMul(llvm::IRBuilder<> &Builder, llvm::Value *Lhs, llvm::Value *Rhs) {
  return Builder.CreateMul(Lhs, Rhs, "multmp");
}

llvm::Value *getInsertElement(llvm::IRBuilder<> &Builder, llvm::Value *Vec, 
                              llvm::Value *Val, llvm::Value *Index) {
  return Builder.CreateInsertElement(Vec, Val, Index, "ins-ele");
}

llvm::Value *getExtractElement(llvm::IRBuilder<> &Builder, llvm::Value *Vec, llvm::Value *Index) {
  return Builder.CreateExtractElement(Vec, Index, "ext-ele");
}

#include "llvm/IR/Verifier.h" // llvm::verifyFunction
int test1(int argc, char **argv) {
  static llvm::IRBuilder<> Builder1(Context1);
  llvm::Function *fooFunc = createFunc1(Builder1, "foo1");

  FuncArgs1.push_back("a");
  setFuncArgs(fooFunc, FuncArgs1);

  llvm::Value *Base = fooFunc->arg_begin();

  llvm::BasicBlock *entry = createBB1(fooFunc, "entry");
  Builder1.SetInsertPoint(entry);

  llvm::Value *gep = getGEP(Builder1, Base, Builder1.getInt32(1));
  llvm::Value *load1 = getLoad(Builder1, gep);

  llvm::Value *constant = Builder1.getInt32(16);
  llvm::Value *val = createArithMul(Builder1, load1, constant);

  // Builder1.CreateRet(load1);
  Builder1.CreateRet(load1);

  llvm::verifyFunction(*fooFunc);
  ModuleOb1->dump();
  return 0;
}

int test2(int argc, char **argv) {
  static llvm::IRBuilder<> Builder2(Context2);
  llvm::Function *fooFunc = createFunc2(Builder2, "foo2");

  FuncArgs2.push_back("a");
  setFuncArgs(fooFunc, FuncArgs2);

  llvm::BasicBlock *entry = createBB2(fooFunc, "entry");
  Builder2.SetInsertPoint(entry);

  llvm::Value *Vec = fooFunc->arg_begin();
  llvm::Value *newVec = nullptr;
  for (unsigned int i=0; i<vectorSize; ++i) {
    newVec = getInsertElement(Builder2, Vec, Builder2.getInt32((i + 1) * 10), Builder2.getInt32(i));
  }

  llvm::SmallVector<llvm::Value*, vectorSize> V;
  V.resize(vectorSize);

  for (unsigned int i=0; i<V.size(); ++i) {
    V[i] = getExtractElement(Builder2, Vec, Builder2.getInt32(i));
  }

  llvm::Value *add1 = createArithMul(Builder2, V[0], V[V.size()-1]);
  llvm::Value *add2 = createArithMul(Builder2, add1, V[V.size()-1]);

  // Builder2.CreateRet(Builder2.getInt32(0));
  Builder2.CreateRet(add2);

  llvm::verifyFunction(*fooFunc);
  ModuleOb2->dump();
  return 0;
}

int main(int argc, char **argv) {
  std::cout << "============= getLoad and getStore =============" << std::endl;
  test1(argc, argv);
  std::cout << "============= getInsertElement =============" << std::endl;
  test2(argc, argv);
  return 0;
}
