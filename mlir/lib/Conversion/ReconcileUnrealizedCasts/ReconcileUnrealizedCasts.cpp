//===- ReconcileUnrealizedCasts.cpp - Eliminate noop unrealized casts -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h"

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"

namespace mlir {
#define GEN_PASS_DEF_RECONCILEUNREALIZEDCASTSPASS
#include "mlir/Conversion/Passes.h.inc"
} // namespace mlir

using namespace mlir;

namespace {

/// Pass to simplify and eliminate unrealized conversion casts.
///
/// This pass processes unrealized_conversion_cast ops in a worklist-driven
/// fashion. For each matched cast op, if the chain of input casts eventually
/// reaches a cast op where the input types match the output types of the
/// matched op, replace the matched op with the inputs.
///
/// Example:
/// %1 = unrealized_conversion_cast %0 : !A to !B
/// %2 = unrealized_conversion_cast %1 : !B to !C
/// %3 = unrealized_conversion_cast %2 : !C to !A
///
/// In the above example, %0 can be used instead of %3 and all cast ops are
/// folded away.
struct ReconcileUnrealizedCasts
    : public impl::ReconcileUnrealizedCastsPassBase<ReconcileUnrealizedCasts> {
  ReconcileUnrealizedCasts() = default;

  void runOnOperation() override {
    SmallVector<UnrealizedConversionCastOp> ops;
    getOperation()->walk(
        [&](UnrealizedConversionCastOp castOp) { ops.push_back(castOp); });
    reconcileUnrealizedCasts(ops);
  }
};

} // namespace
