; RUN: opt -O0 -S test.ll > 0.ll
;      opt -O1 -S test.ll > 1.ll
;      opt -O2 -S test.ll > 2.ll
;      opt -O3 -S test.ll > 3.ll
; And see the difference in the optimized code for the O0, O1 and O2 levels.

define internal i32 @test(i32* %X, i32* %Y)
{
  %A = load i32, i32* %X
  %B = load i32, i32* %Y
  %C = add i32 %A, %B
  ret i32 %C
}

define internal i32 @caller(i32* %B)
{
  %A = alloca i32
  store i32 1, i32* %A
  %C = call i32 @test(i32* %A, i32* %B)
  ret i32 %C
}

define i32 @callercaller()
{
  %B = alloca i32
  store i32 2, i32* %B
  %X = call i32 @caller(i32* %B)
  ret i32 %X
}

; RUN: opt -passes="instcombine" -S ../test.ll
;      opt -S ../test.ll
; And see the difference in the optimized code.
; It has removed one redundant add instruction and hence combined the two
; `add` instructions to one.
define i32 @instcombine(i32 %X)
{
  %Y = add i32 %X, 1 ; |
  %Z = add i32 %Y, 1 ; | --> %Z = add i32 %X, 2
  ret i32 %Z
}

define i32 @testfunc(i32 %x, i32 %y, i32 %z)
{
  %xor1 = xor i32 %y, %z    ; |
  %or = or i32 %x, %xor1    ; |
  %xor2 = xor i32 %x, %z    ; |
  %xor3 = xor i32 %xor2, %y ; |     %xor1 = xor i32 %y, %z
  %res = xor i32 %or, %xor3 ; | --> %1 = and i32 %xor1, %x
  ret i32 %res
}
