#ifndef SRC_CPU_H_
#define SRC_CPU_H_

class Cpu {
public:
    Cpu(const int&, const int&);
    virtual ~Cpu();
    int getAC() const;
private:
    int ac;
};

#endif
