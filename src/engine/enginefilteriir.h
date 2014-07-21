#ifndef ENGINEFILTERIIR_H
#define ENGINEFILTERIIR_H

#include "engine/engineobject.h"
#include "util/types.h"

#define IIR_LP 0
#define IIR_BP 1
#define IIR_HP 2

template<unsigned int SIZE, unsigned int PASS>
class EngineFilterIIR : public EngineObjectConstIn {
  public:
    EngineFilterIIR()
            : m_doRamping(false){
        memset(m_coef, 0, (SIZE + 1) * sizeof(double));
        initBuffers();
    }

    virtual ~EngineFilterIIR() {};

    void initBuffers() {
        // Copy the current buffers into the old buffers
        memcpy(m_oldBuf1, m_buf1, SIZE * sizeof(double));
        memcpy(m_oldBuf2, m_buf2, SIZE * sizeof(double));
        // Set the current buffers to 0
        memset(m_buf1, 0, SIZE * sizeof(double));
        memset(m_buf2, 0, SIZE * sizeof(double));
    }

    virtual void process(const CSAMPLE* pIn, CSAMPLE* pOutput,
                                     const int iBufferSize) {
        double tmp1, tmp2;
        double cross_mix = 0.0;
        double cross_inc = 2.0 / static_cast<double>(iBufferSize);
        for (int i = 0; i < iBufferSize; i += 2) {
            pOutput[i] = processSample(m_coef, m_buf1, pIn[i]);
            pOutput[i+1] = processSample(m_coef, m_buf2, pIn[i + 1]);
            // Do a linear cross fade between the old samples and the new samples
            if (m_doRamping) {
                tmp1 = processSample(m_oldCoef, m_oldBuf1, pIn[i]);
                tmp2 = processSample(m_oldCoef, m_oldBuf2, pIn[i + 1]);
                pOutput[i] = pOutput[i] * cross_mix +
                             tmp1 * (1.0 - cross_mix);
                pOutput[i + 1] = pOutput[i + 1] * cross_mix +
                             tmp2 * (1.0 - cross_mix);
                cross_mix += cross_inc;
            }
        }
        m_doRamping = false;
    }

  protected:
    inline double processSample(double* coef, double* buf, register double val);

    double m_coef[SIZE + 1];
    // Old coefficients needed for ramping
    double m_oldCoef[SIZE + 1];

    // Channel 1 state
    double m_buf1[SIZE];
    // Old channel 1 buffer needed for ramping
    double m_oldBuf1[SIZE];

    // Channel 2 state
    double m_buf2[SIZE];
    // Old channel 2 buffer needed for ramping
    double m_oldBuf2[SIZE];

    // Flag set to true if ramping needs to be done
    bool m_doRamping;
};

template<>
inline double EngineFilterIIR<4, IIR_LP>::processSample(double* coef, double* buf, register double val) {
    register double tmp, fir, iir;
    tmp = buf[0]; buf[0] = buf[1]; buf[1] = buf[2]; buf[2] = buf[3];
    iir = val * coef[0];
    iir -= coef[1] * tmp; fir = tmp;
    iir -= coef[2] * buf[0]; fir += buf[0] + buf[0];
    fir += iir;
    tmp = buf[1]; buf[1] = iir; val = fir;
    iir = val;
    iir -= coef[3] * tmp; fir = tmp;
    iir -= coef[4] * buf[2]; fir += buf[2] + buf[2];
    fir += iir;
    buf[3] = iir; val = fir;
    return val;
}

template<>
inline double EngineFilterIIR<8, IIR_BP>::processSample(double* coef, double* buf, register double val) {
    register double tmp, fir, iir;
    tmp = buf[0]; buf[0] = buf[1]; buf[1] = buf[2]; buf[2] = buf[3];
    buf[3] = buf[4]; buf[4] = buf[5]; buf[5] = buf[6]; buf[6] = buf[7];
    iir = val * coef[0];
    iir -= coef[1] * tmp; fir = tmp;
    iir -= coef[2] * buf[0]; fir += -buf[0] - buf[0];
    fir += iir;
    tmp = buf[1]; buf[1] = iir; val= fir;
    iir = val;
    iir -= coef[3] * tmp; fir = tmp;
    iir -= coef[4] * buf[2]; fir += -buf[2] - buf[2];
    fir += iir;
    tmp = buf[3]; buf[3] = iir; val= fir;
    iir = val;
    iir -= coef[5] * tmp; fir = tmp;
    iir -= coef[6] * buf[4]; fir += buf[4] + buf[4];
    fir += iir;
    tmp = buf[5]; buf[5] = iir; val= fir;
    iir = val;
    iir -= coef[7] * tmp; fir = tmp;
    iir -= coef[8] * buf[6]; fir += buf[6] + buf[6];
    fir += iir;
    buf[7] = iir; val = fir;
    return val;
}

template<>
inline double EngineFilterIIR<4, IIR_HP>::processSample(double* coef, double* buf, register double val) {
    register double tmp, fir, iir;
    tmp = buf[0]; buf[0] = buf[1]; buf[1] = buf[2]; buf[2] = buf[3];
    iir= val * coef[0];
    iir -= coef[1] * tmp; fir = tmp;
    iir -= coef[2] * buf[0]; fir += -buf[0] - buf[0];
    fir += iir;
    tmp = buf[1]; buf[1] = iir; val = fir;
    iir = val;
    iir -= coef[3] * tmp; fir = tmp;
    iir -= coef[4] * buf[2]; fir += -buf[2] - buf[2];
    fir += iir;
    buf[3] = iir; val = fir;
    return val;
}

template<>
inline double EngineFilterIIR<8, IIR_LP>::processSample(double* coef, double* buf, register double val) {
    register double tmp, fir, iir;
    tmp = buf[0]; buf[0] = buf[1]; buf[1] = buf[2]; buf[2] = buf[3];
    buf[3] = buf[4]; buf[4] = buf[5]; buf[5] = buf[6]; buf[6] = buf[7];
    iir = val * coef[0];
    iir -= coef[1] * tmp; fir = tmp;
    iir -= coef[2] * buf[0]; fir += buf[0] + buf[0];
    fir += iir;
    tmp = buf[1]; buf[1] = iir; val = fir;
    iir = val;
    iir -= coef[3] * tmp; fir = tmp;
    iir -= coef[4] * buf[2]; fir += buf[2] + buf[2];
    fir += iir;
    tmp = buf[3]; buf[3] = iir; val = fir;
    iir = val;
    iir -= coef[5] * tmp; fir = tmp;
    iir -= coef[6] * buf[4]; fir += buf[4] + buf[4];
    fir += iir;
    tmp = buf[5]; buf[5] = iir; val = fir;
    iir = val;
    iir -= coef[7] * tmp; fir = tmp;
    iir -= coef[8] * buf[6]; fir += buf[6] + buf[6];
    fir += iir;
    buf[7] = iir; val = fir;
    return val;
}

template<>
inline double EngineFilterIIR<16, IIR_BP>::processSample(double* coef, double* buf, register double val) {
    register double tmp, fir, iir;
    tmp = buf[0]; buf[0] = buf[1]; buf[1] = buf[2]; buf[2] = buf[3];
    buf[3] = buf[4]; buf[4] = buf[5]; buf[5] = buf[6]; buf[6] = buf[7];
    buf[7] = buf[8]; buf[8] = buf[9]; buf[9] = buf[10]; buf[10] = buf[11];
    buf[11] = buf[12]; buf[12] = buf[13]; buf[13] = buf[14]; buf[14] = buf[15];
    iir = val * coef[0];
    iir -= coef[1] * tmp; fir = tmp;
    iir -= coef[2] * buf[0]; fir += -buf[0] - buf[0];
    fir += iir;
    tmp = buf[1]; buf[1] = iir; val = fir;
    iir = val;
    iir -= coef[3] * tmp; fir = tmp;
    iir -= coef[4] * buf[2]; fir += -buf[2] - buf[2];
    fir += iir;
    tmp = buf[3]; buf[3] = iir; val = fir;
    iir = val;
    iir -= coef[5] * tmp; fir = tmp;
    iir -= coef[6] * buf[4]; fir += -buf[4] - buf[4];
    fir += iir;
    tmp = buf[5]; buf[5] = iir; val = fir;
    iir = val;
    iir -= coef[7] * tmp; fir = tmp;
    iir -= coef[8] * buf[6]; fir += -buf[6] - buf[6];
    fir += iir;
    tmp = buf[7]; buf[7]= iir; val= fir;
    iir = val;
    iir -= coef[9] * tmp; fir = tmp;
    iir -= coef[10] * buf[8]; fir += buf[8] + buf[8];
    fir += iir;
    tmp = buf[9]; buf[9] = iir; val = fir;
    iir = val;
    iir -= coef[11] * tmp; fir = tmp;
    iir -= coef[12] * buf[10]; fir += buf[10] + buf[10];
    fir += iir;
    tmp = buf[11]; buf[11] = iir; val = fir;
    iir = val;
    iir -= coef[13] * tmp; fir = tmp;
    iir -= coef[14] * buf[12]; fir += buf[12] + buf[12];
    fir += iir;
    tmp = buf[13]; buf[13] = iir; val = fir;
    iir = val;
    iir -= coef[15] * tmp; fir = tmp;
    iir -= coef[16] * buf[14]; fir += buf[14] + buf[14];
    fir += iir;
    buf[15] = iir; val = fir;
    return val;
}

template<>
inline double EngineFilterIIR<8, IIR_HP>::processSample(double* coef, double* buf, register double val) {
    register double tmp, fir, iir;
    tmp = buf[0]; buf[0] = buf[1]; buf[1] = buf[2]; buf[2] = buf[3];
    buf[3] = buf[4]; buf[4] = buf[5]; buf[5] = buf[6]; buf[6] = buf[7];
    iir = val * coef[0];
    iir -= coef[1] * tmp; fir = tmp;
    iir -= coef[2] * buf[0]; fir += -buf[0] - buf[0];
    fir += iir;
    tmp = buf[1]; buf[1] = iir; val = fir;
    iir = val;
    iir -= coef[3] * tmp; fir = tmp;
    iir -= coef[4] * buf[2]; fir += -buf[2] - buf[2];
    fir += iir;
    tmp = buf[3]; buf[3] = iir; val = fir;
    iir = val;
    iir -= coef[5] * tmp; fir = tmp;
    iir -= coef[6] * buf[4]; fir += -buf[4] - buf[4];
    fir += iir;
    tmp = buf[5]; buf[5] = iir; val = fir;
    iir = val;
    iir -= coef[7] * tmp; fir = tmp;
    iir -= coef[8] * buf[6]; fir += -buf[6] - buf[6];
    fir += iir;
    buf[7] = iir; val = fir;
    return val;
}



#endif // ENGINEFILTERIIR_H
