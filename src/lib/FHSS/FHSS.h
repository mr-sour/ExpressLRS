#pragma once

#include "targets.h"
#include "random.h"

#if defined(RADIO_SX127X)
#define FreqCorrectionMax ((int32_t)(100000/FREQ_STEP))
#elif defined(RADIO_SX128X)
#define FreqCorrectionMax ((int32_t)(200000/FREQ_STEP))
#endif
#define FreqCorrectionMin (-FreqCorrectionMax)

#define FREQ_HZ_TO_REG_VAL(freq) ((uint32_t)((double)freq/(double)FREQ_STEP))
#define FREQ_SPREAD_SCALE 256

typedef struct {
    const char  *domain;
    uint32_t    freq_start_1;
    uint32_t    freq_stop_1;
    uint32_t    freq_count_1;
    uint32_t    freq_start_2;
    uint32_t    freq_stop_2;
    uint32_t    freq_count_2;
} fhss_config_t;

extern volatile uint8_t FHSSptr;
extern uint32_t freq_spread;
extern int32_t FreqCorrection;
extern int32_t FreqCorrection_2;
extern uint8_t FHSSsequence[];
extern uint_fast8_t sync_channel;
extern const fhss_config_t *FHSSconfig;

// create and randomise an FHSS sequence
void FHSSrandomiseFHSSsequence(uint32_t seed);

// The number of frequencies for this regulatory domain for Radio 1
static inline uint32_t FHSSgetChannelCount_1(void)
{
    return FHSSconfig->freq_count_1;
}

// The number of frequencies for this regulatory domain for Radio 2
static inline uint32_t FHSSgetChannelCount_2(void)
{
    return FHSSconfig->freq_count_2;
}

// get the number of entries in the FHSS sequence for Radio 1
static inline uint16_t FHSSgetSequenceCount_1()
{
    return (256 / FHSSconfig->freq_count_1) * FHSSconfig->freq_count_1;
}

// get the number of entries in the FHSS sequence for Radio 2
static inline uint16_t FHSSgetSequenceCount_2()
{
    return (256 / FHSSconfig->freq_count_2) * FHSSconfig->freq_count_2;
}

// get the initial frequency for Radio 1, which is also the sync channel
static inline uint32_t GetInitialFreq_1()
{
    return FHSSconfig->freq_start_1 + (sync_channel * freq_spread / FREQ_SPREAD_SCALE) - FreqCorrection;
}

// get the initial frequency for Radio 2, which is also the sync channel
static inline uint32_t GetInitialFreq_2()
{
    return FHSSconfig->freq_start_2 + (sync_channel * freq_spread / FREQ_SPREAD_SCALE) - FreqCorrection_2;
}

// Get the current sequence pointer
static inline uint8_t FHSSgetCurrIndex()
{
    return FHSSptr;
}

// Set the sequence pointer, used by RX on SYNC
static inline void FHSSsetCurrIndex(const uint8_t value)
{
    FHSSptr = value % FHSSgetSequenceCount_1(); // Assuming Radio 1 sequence count
}

// Advance the pointer to the next hop and return the frequency of that channel for Radio 1
static inline uint32_t FHSSgetNextFreq_1()
{
    FHSSptr = (FHSSptr + 1) % FHSSgetSequenceCount_1();
    uint32_t freq = FHSSconfig->freq_start_1 + (freq_spread * FHSSsequence[FHSSptr] / FREQ_SPREAD_SCALE) - FreqCorrection;
    return freq;
}

// Advance the pointer to the next hop and return the frequency of that channel for Radio 2
static inline uint32_t FHSSgetNextFreq_2()
{
    FHSSptr = (FHSSptr + 1) % FHSSgetSequenceCount_2();
    uint32_t freq = FHSSconfig->freq_start_2 + (freq_spread * FHSSsequence[FHSSptr] / FREQ_SPREAD_SCALE) - FreqCorrection_2;
    return freq;
}

static inline const char *getRegulatoryDomain()
{
    return FHSSconfig->domain;
}

// Get frequency offset by half of the domain frequency range for Radio 1
static inline uint32_t FHSSGeminiFreq_1(uint8_t FHSSsequenceIdx)
{
    uint32_t numfhss = FHSSgetChannelCount_1();
    uint8_t offSetIdx = (FHSSsequenceIdx + (numfhss / 2)) % numfhss;  
    uint32_t freq = FHSSconfig->freq_start_1 + (freq_spread * offSetIdx / FREQ_SPREAD_SCALE) - FreqCorrection;
    return freq;
}

// Get frequency offset by half of the domain frequency range for Radio 2
static inline uint32_t FHSSGeminiFreq_2(uint8_t FHSSsequenceIdx)
{
    uint32_t numfhss = FHSSgetChannelCount_2();
    uint8_t offSetIdx = (FHSSsequenceIdx + (numfhss / 2)) % numfhss;  
    uint32_t freq = FHSSconfig->freq_start_2 + (freq_spread * offSetIdx / FREQ_SPREAD_SCALE) - FreqCorrection_2;
    return freq;
}

// Get the Gemini frequency for Radio 1
static inline uint32_t FHSSgetGeminiFreq_1()
{
    return FHSSGeminiFreq_1(FHSSsequence[FHSSgetCurrIndex()]);
}

// Get the Gemini frequency for Radio 2
static inline uint32_t FHSSgetGeminiFreq_2()
{
    return FHSSGeminiFreq_2(FHSSsequence[FHSSgetCurrIndex()]);
}

// Get the initial Gemini frequency for Radio 1
static inline uint32_t FHSSgetInitialGeminiFreq_1()
{
    return FHSSGeminiFreq_1(sync_channel);
}

// Get the initial Gemini frequency for Radio 2
static inline uint32_t FHSSgetInitialGeminiFreq_2()
{
    return FHSSGeminiFreq_2(sync_channel);
}
