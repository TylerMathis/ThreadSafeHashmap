#include <iostream>
#include "../src/MarkableReference.h"

using std::cout;

int main() {
    cout << "\n\nTESTING MARKABLE REFERENCE...\n\n";

    MarkableReference<int> markable;

    // Construct
    assert(markable.getRef() == nullptr);
    assert(markable.getMark() == false);

    // Mark ops
    markable.setMark(true);
    assert(markable.getMark() == true);

    bool oldMark = markable.exchangeMark(false);
    assert(oldMark == true && markable.getMark() == false);

    // Ref ops
    int newVal = 5;
    int *newRef = &newVal;
    int *oldRef = markable.exchangeRef(newRef);
    assert(oldRef == nullptr && *markable.getRef() == *newRef);

    // Comp exchange weak
    int *expectedRef = newRef;
    bool expectedMark = false;
    *newRef = 10;

    markable.compareExchangeBothWeak(expectedRef, expectedMark, newRef, true);

    bool resMark;
    int *resRef = markable.getBoth(resMark);

    assert(resMark == true && *resRef == *newRef);

    cout << "\nSuccess :D\n";

    return 0;
}

