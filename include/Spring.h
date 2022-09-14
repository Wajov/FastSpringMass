#ifndef SPRING_H
#define SPRING_H

class Spring {
private:
    int source, target;
    float length;

public:
    Spring(const int source, const int target, const float length);
    ~Spring();
    int getSource() const;
    int getTarget() const;
    float getLength() const;
};

#endif