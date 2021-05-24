#ifndef FFTW_PLAN_H
#define FFTW_PLAN_H

#include <iostream>
#include <vector>

#include <fftw3.h>

class fftwPlan {
private:
    float* in;
    fftwf_complex* out;
    fftwf_plan p;
    long long size;

public:
    fftwPlan(long long size) : size(size) {
        in = (float*) fftwf_malloc(sizeof(float) * size);
        out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (size / 2 + 1));
        p = fftwf_plan_dft_r2c_1d(size, in, (fftwf_complex*)out, FFTW_MEASURE);
    };
    ~fftwPlan() {
        fftwf_destroy_plan(p);
        fftwf_free(in);
        fftwf_free(out);
    }

    float* getIn() { return in; }
    fftwf_complex* getOut() { return out; }
    long long getInSize() { return size; }
    long long getOutSize() { return size / 2 + 1; }
    void execute() { fftwf_execute(p); }

    static int loadWisdom(const char* filename) {
        return fftwf_import_wisdom_from_filename(filename);
    }
    static int saveWisdom(const char* filename) {
        return fftwf_export_wisdom_to_filename(filename);
    }
};


#endif