
// RUN: clang -emit-llvm -S intrinsic.cpp
//      opt -O1 -S intrinsic.ll > O1-Optimized-intrinsic.ll
int func()
{
  int a[5];
  for (int i = 0; i != 5; ++i)
    a[i] = 0;
  return a[0];
}

int a[4], b[4], c[4];
void addsub()
{
  a[0] = b[0] + c[0];
  a[1] = b[1] + c[1];
  a[2] = b[2] + c[2];
  a[3] = b[3] + c[3];
}
