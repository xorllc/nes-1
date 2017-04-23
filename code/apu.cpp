﻿#include "apu.h"

#pragma region Pulse

internal void StepPulseEvenlope(APU::Pulse *pulse)
{
    if (pulse->envelopeStart)
    {
        pulse->envelopeVolume = 15;
        pulse->envelopeValue = pulse->envelopePeriod;
        pulse->envelopeStart = FALSE;
    }
    else if (pulse->envelopeValue > 0)
    {
        pulse->envelopeValue--;
    }
    else
    {
        if (pulse->envelopeVolume > 0)
        {
            pulse->envelopeVolume--;
        }
        else if (pulse->envelopeLoop)
        {
            pulse->envelopeVolume = 15;
        }

        pulse->envelopeValue = pulse->envelopePeriod;
    }
}

internal void PulseSweep(APU::Pulse *pulse)
{
    u8 delta = pulse->timerPeriod >> pulse->sweepShift;

    if (pulse->sweepNegate)
    {
        pulse->timerPeriod -= delta;

        // The two pulse channels have their adders' carry inputs wired differently, which produces different results when each channel's change amount is made negative :
        // Pulse 1 adds the ones' complement (−c − 1). Making 20 negative produces a change amount of −21.
        // Pulse 2 adds the two's complement (−c). Making 20 negative produces a change amount of −20.
        //
        // from: http://wiki.nesdev.com/w/index.php/APU_Sweep
        if (pulse->channel == 1)
        {
            pulse->timerPeriod--;
        }
    }
    else
    {
        pulse->timerPeriod += delta;
    }
}

internal void StepPulseSweep(APU::Pulse *pulse)
{
    if (pulse->sweepReload)
    {
        if (pulse->sweepEnabled && !pulse->sweepValue)
        {
            PulseSweep(pulse);
        }

        pulse->sweepValue = pulse->sweepPeriod;
        pulse->sweepReload = FALSE;
    }
    else if (pulse->sweepValue > 0)
    {
        pulse->sweepValue--;
    }
    else
    {
        if (pulse->sweepEnabled)
        {
            PulseSweep(pulse);
        }

        pulse->sweepValue = pulse->sweepPeriod;
    }
}

internal void StepPulseTimer(APU::Pulse *pulse)
{
    if (!pulse->timerValue)
    {
        pulse->timerValue = pulse->timerPeriod;
        pulse->dutyValue = (pulse->dutyValue + 1) % 8;
    }
    else
    {
        pulse->timerValue--;
    }
}

internal void StepPulseLength(APU::Pulse *pulse)
{
    if (pulse->lengthEnabled && pulse->lengthValue > 0)
    {
        pulse->lengthValue--;
    }
}

internal u8 GetPulseOutput(APU::Pulse *pulse)
{
    if (!pulse->enabled)
    {
        return 0;
    }

    if (!pulse->lengthValue)
    {
        return 0;
    }

    if (!dutyTable[pulse->dutyMode][pulse->dutyValue])
    {
        return 0;
    }

    if (pulse->timerPeriod < 8 || pulse->timerPeriod > 0x7FF)
    {
        return 0;
    }

    if (pulse->envelopeEnabled)
    {
        return pulse->envelopeVolume;
    }

    return pulse->constantVolume;
}

#pragma endregion

#pragma region Triangle

#pragma endregion

#pragma region Noise

#pragma endregion

#pragma region Dmc

#pragma endregion

internal void StepAPUTimer(NES *nes)
{
    APU *apu = &nes->apu;
    if (!(apu->cycles & 1))
    {
        StepPulseTimer(&apu->pulse1);
        StepPulseTimer(&apu->pulse2);
        // StepNoiseTimer(&apu->noise);
        // StepDmcTimer(&apu->dmc);
    }
    // StepTriangleTimer(&apu->triangle);
}

internal void StepAPUEnvelope(APU *apu)
{
    StepPulseEvenlope(&apu->pulse1);
    StepPulseEvenlope(&apu->pulse2);
    // StepTriangleEnvelope(&apu->triangle);
    // StepNoiseEnvelope(&apu->noise);
}

internal void StepAPUSweep(APU *apu)
{
    StepPulseSweep(&apu->pulse1);
    StepPulseSweep(&apu->pulse2);
}

internal void StepAPUTimer(APU *apu)
{
    if (!(apu->cycles & 1))
    {
        StepPulseTimer(&apu->pulse1);
        StepPulseTimer(&apu->pulse2);
        // StepNoiseTimer(&apu->noise);
        // StepDmcTimer(&apu->dmc);
    }

    // StepTriangleTimer(&apu->triangle);
}

internal void StepAPULength(APU *apu)
{
    StepPulseLength(&apu->pulse1);
    StepPulseLength(&apu->pulse2);
    // StepTriangleLength(&apu->triangle);
    // StepNoiseLength(&apu->noise);
}

// mode 0:    mode 1:       function
// ---------  -----------  -----------------------------
//  - - - f    - - - - -    IRQ (if bit 6 is clear)
//  - l - l    l - l - -    Length counter and sweep
//  e e e e    e e e e -    Envelope and linear counter
// from: http://wiki.nesdev.com/w/index.php/APU#Frame_Counter_.28.244017.29
internal void StepAPUFrameCounter(NES *nes)
{
    APU *apu = &nes->apu;

    if (apu->frameMode)
    {
        // 5-step mode
        apu->frameValue = (apu->frameValue + 1) % 5;

        switch (apu->frameValue)
        {
            case 0:
            {
                StepAPUEnvelope(apu);
                StepAPUSweep(apu);
                StepAPULength(apu);
                break;
            }

            case 1:
            {
                StepAPUEnvelope(apu);
                break;
            }

            case 2:
            {
                StepAPUEnvelope(apu);
                StepAPUSweep(apu);
                StepAPULength(apu);
                break;
            }

            case 3:
            {
                StepAPUEnvelope(apu);
                break;
            }

            case 4:
            {
                break;
            }
        }
    }
    else
    {
        // 4-step mode
        apu->frameValue = (apu->frameValue + 1) % 4;

        switch (apu->frameValue)
        {
            case 0:
            {
                StepAPUEnvelope(apu);
                break;
            }

            case 1:
            {
                StepAPUEnvelope(apu);
                StepAPUSweep(apu);
                StepAPULength(apu);
                break;
            }

            case 2:
            {
                StepAPUEnvelope(apu);
                break;
            }

            case 3:
            {
                StepAPUEnvelope(apu);
                StepAPUSweep(apu);
                StepAPULength(apu);

                if (apu->frameIRQ)
                {
                    if (!(nes->cpu.p & 4))
                    {
                        nes->cpu.interrupt = CPU_INTERRUPT_IRQ;
                    }
                }
                break;
            }
        }
    }
}

internal f32 HighPassFilter(f32 sampleValue, s32 freq)
{
    local f32 lastInputSample, lastInputSample2, lastOutputSample;

    f32 a = (f32)freq / ((f32)freq + (f32)APU_SAMPLES_PER_SECOND);
    f32 newSampleValue = a * (lastOutputSample + lastInputSample - lastInputSample2);

    lastInputSample2 = lastInputSample;
    lastInputSample = sampleValue;
    lastOutputSample = newSampleValue;

    return newSampleValue;
}

internal f32 LowPassFilter(f32 sampleValue, s32 freq)
{
    local f32 lastInputSample, lastOutputSample;

    f32 a = (f32)APU_SAMPLES_PER_SECOND / ((f32)freq + (f32)APU_SAMPLES_PER_SECOND);
    f32 newSampleValue = lastOutputSample + a * (lastInputSample - lastOutputSample);

    lastInputSample = sampleValue;
    lastOutputSample = newSampleValue;

    return newSampleValue;
}

internal f32 GetOutput(APU *apu)
{
    u8 p1 = GetPulseOutput(&apu->pulse1);
    u8 p2 = GetPulseOutput(&apu->pulse2);
    /*f32 t = GetTriangleOutput(&apu->triangle);
    f32 n = GetNoiseOutput(&apu->noise);
    f32 d = GetDmcOutput(&apu->dmc);*/

    f32 pulseOut = pulseTable[p1 + p2];
    f32 tndOut = 0; // tndTable[3 * t + 2 * n + d];
    return pulseOut + tndOut;
}

void StepAPU(NES *nes)
{
    APU *apu = &nes->apu;

    StepAPUTimer(nes);

    apu->frameCounter++;

    if (apu->frameCounter >= FRAME_COUNTER_RATE)
    {
        StepAPUFrameCounter(nes);
        apu->frameCounter = 0;
    }

    apu->sampleCounter++;

    if (apu->sampleCounter >= apu->sampleRate)
    {
        f32 output = GetOutput(apu);
        output = HighPassFilter(output, 90);
        output = HighPassFilter(output, 440);
        output = LowPassFilter(output, 14000);

        apu->buffer[apu->bufferIndex] = output * 40000;
        apu->bufferIndex = (apu->bufferIndex + 1) % APU_BUFFER_LENGTH;

        apu->sampleCounter = 0;
    }

    apu->cycles++;
}

void WriteAPUFrameCounter(NES *nes, u8 value)
{
    APU *apu = &nes->apu;

    apu->frameMode = (value & 0x80) >> 7;

    if (value & 0x40)
    {
        apu->frameIRQ = FALSE;
    }
    else if (!apu->frameMode)
    {
        apu->frameIRQ = TRUE;
    }

    apu->frameValue = 0;

    if (apu->frameMode)
    {
        StepAPUEnvelope(apu);
        StepAPUSweep(apu);
        StepAPULength(apu);
    }
}

void ResetAPU(NES *nes)
{
    APU *apu = &nes->apu;

    apu->frameCounter = 0;
    apu->sampleCounter = 0;

    apu->pulse1.enabled = FALSE;
    apu->pulse1.lengthValue = 0;

    apu->pulse2.enabled = FALSE;
    apu->pulse2.lengthValue = 0;

    apu->pulse1.channel = 1;
    apu->pulse2.channel = 2;
    // apu->noise.shiftRegister = 1

    apu->bufferIndex = 0;
    apu->sampleRate = APU_CYCLES_PER_SAMPLE;
    memset(apu->buffer, 0, sizeof(apu->buffer));

    WriteAPUFrameCounter(nes, 0);
}

void PowerAPU(NES *nes)
{
    ResetAPU(nes);
}

void InitAPU(NES *nes)
{
    // from: http://wiki.nesdev.com/w/index.php/APU_Mixer
    //
    // pulse_table[n] = 95.52 / (8128.0 / n + 100)
    // tnd_table[n] = 163.67 / (24329.0 / n + 100)
    //
    // output = pulse_out + tnd_out
    // pulse_out = pulse_table[pulse1 + pulse2]
    // tnd_out = tnd_table[3 * triangle + 2 * noise + dmc]

    for (s32 i = 0; i < 31; ++i)
    {
        pulseTable[i] = 95.52f / (8128.0f / (f32)i + 100.0f);
    }

    for (s32 i = 0; i < 203; ++i)
    {
        tndTable[i] = 163.67f / (24329.0f / (f32)i + 100.0f);
    }

    APU *apu = &nes->apu;
    apu->pulse1.channel = 1;
    apu->pulse2.channel = 2;
}
