// Copyright 2013 the V8 project authors. All rights reserved.

// Check that we can traverse very deep stacks of ConsStrings using
// StringCharacterStram.  Check that Get(int) works on very deep stacks
// of ConsStrings.  These operations may not be very fast, but they
// should be possible without getting errors due to too deep recursion.

#include "src/v8.h"

#include "src/objects.h"
#include "src/ostreams.h"
#include "test/cctest/cctest.h"

using namespace v8::internal;


TEST(Create) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  const int kNumSymbols = 30;
  Handle<Symbol> symbols[kNumSymbols];

  OFStream os(stdout);
  for (int i = 0; i < kNumSymbols; ++i) {
    symbols[i] = isolate->factory()->NewSymbol();
    CHECK(symbols[i]->IsName());
    CHECK(symbols[i]->IsSymbol());
    CHECK(symbols[i]->HasHashCode());
    CHECK_GT(symbols[i]->Hash(), 0u);
    os << Brief(*symbols[i]) << "\n";
#if OBJECT_PRINT
    symbols[i]->Print(os);
#endif
#if VERIFY_HEAP
    symbols[i]->ObjectVerify();
#endif
  }

  CcTest::heap()->CollectGarbage(i::NEW_SPACE);
  CcTest::heap()->CollectAllGarbage(Heap::kNoGCFlags);

  // All symbols should be distinct.
  for (int i = 0; i < kNumSymbols; ++i) {
    CHECK(symbols[i]->SameValue(*symbols[i]));
    for (int j = i + 1; j < kNumSymbols; ++j) {
      CHECK(!symbols[i]->SameValue(*symbols[j]));
    }
  }
}
