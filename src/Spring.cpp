#include "Spring.h"

Spring::Spring(const int source, const int target, const float length) :
    source(source),
    target(target),
    length(length) {}

Spring::~Spring() {}

int Spring::getSource() const {
    return source;
}

int Spring::getTarget() const {
    return target;
}

float Spring::getLength() const {
    return length;
}