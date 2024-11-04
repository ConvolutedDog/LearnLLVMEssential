#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Config/llvm-config.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

#include "llvm/IR/Function.h"

namespace {
struct FnNamePrintPass : public llvm::PassInfoMixin<FnNamePrintPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &AM) {
    llvm::errs() << F.getName() << "\n";
    return llvm::PreservedAnalyses::all();
  }
};

// After LLVM v9.0, use the following registering method:
llvm::PassPluginLibraryInfo getFnNamePrintPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "FnNamePrintPass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "funcnameprint") {
                    FPM.addPass(FnNamePrintPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getFnNamePrintPassPluginInfo();
}
}
