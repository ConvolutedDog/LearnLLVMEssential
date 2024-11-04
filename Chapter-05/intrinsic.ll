; ModuleID = 'intrinsic.cpp'
source_filename = "intrinsic.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@a = dso_local global [4 x i32] zeroinitializer, align 16
@b = dso_local global [4 x i32] zeroinitializer, align 16
@c = dso_local global [4 x i32] zeroinitializer, align 16

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local noundef i32 @_Z4funcv() #0 {
entry:
  %a = alloca [5 x i32], align 16
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, ptr %i, align 4
  %cmp = icmp ne i32 %0, 5
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32, ptr %i, align 4
  %idxprom = sext i32 %1 to i64
  %arrayidx = getelementptr inbounds [5 x i32], ptr %a, i64 0, i64 %idxprom
  store i32 0, ptr %arrayidx, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %2 = load i32, ptr %i, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, ptr %i, align 4
  br label %for.cond, !llvm.loop !6

for.end:                                          ; preds = %for.cond
  %arrayidx1 = getelementptr inbounds [5 x i32], ptr %a, i64 0, i64 0
  %3 = load i32, ptr %arrayidx1, align 16
  ret i32 %3
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @_Z6addsubv() #0 {
entry:
  %0 = load i32, ptr @b, align 16
  %1 = load i32, ptr @c, align 16
  %add = add nsw i32 %0, %1
  store i32 %add, ptr @a, align 16
  %2 = load i32, ptr getelementptr inbounds ([4 x i32], ptr @b, i64 0, i64 1), align 4
  %3 = load i32, ptr getelementptr inbounds ([4 x i32], ptr @c, i64 0, i64 1), align 4
  %add1 = add nsw i32 %2, %3
  store i32 %add1, ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 1), align 4
  %4 = load i32, ptr getelementptr inbounds ([4 x i32], ptr @b, i64 0, i64 2), align 8
  %5 = load i32, ptr getelementptr inbounds ([4 x i32], ptr @c, i64 0, i64 2), align 8
  %add2 = add nsw i32 %4, %5
  store i32 %add2, ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 2), align 8
  %6 = load i32, ptr getelementptr inbounds ([4 x i32], ptr @b, i64 0, i64 3), align 4
  %7 = load i32, ptr getelementptr inbounds ([4 x i32], ptr @c, i64 0, i64 3), align 4
  %add3 = add nsw i32 %6, %7
  store i32 %add3, ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 3), align 4
  ret void
}

attributes #0 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 20.0.0git (https://github.com/llvm/llvm-project.git fbec1c2a08ce2ae9750ddf3cecc86c5dd2bbc9d8)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
