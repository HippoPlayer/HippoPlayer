/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2011-2017 Leandro Nini <drfiemost@users.sourceforge.net>
 * Copyright 2007-2010 Antti Lankila
 * Copyright 2004,2010 Dag Lem <resid@nimrod.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef ENVELOPEGENERATOR_H
#define ENVELOPEGENERATOR_H

#include "siddefs-fp.h"

namespace reSIDfp
{

/**
 * A 15 bit [LFSR] is used to implement the envelope rates, in effect dividing
 * the clock to the envelope counter by the currently selected rate period.
 *
 * In addition, another 5 bit counter is used to implement the exponential envelope decay,
 * in effect further dividing the clock to the envelope counter.
 * The period of this counter is set to 1, 2, 4, 8, 16, 30 at the envelope counter
 * values 255, 93, 54, 26, 14, 6, respectively.
 *
 * [LFSR]: https://en.wikipedia.org/wiki/Linear_feedback_shift_register
 */
class EnvelopeGenerator
{
private:
    /**
     * The envelope state machine's distinct states. In addition to this,
     * envelope has a hold mode, which freezes envelope counter to zero.
     */
    enum State
    {
        ATTACK, DECAY_SUSTAIN, RELEASE, FREEZED
    };

private:
    /// XOR shift register for ADSR prescaling.
    unsigned int lfsr;

    /// Comparison value (period) of the rate counter before next event.
    unsigned int rate;

    /**
     * During release mode, the SID arpproximates envelope decay via piecewise
     * linear decay rate.
     */
    unsigned int exponential_counter;

    /**
     * Comparison value (period) of the exponential decay counter before next
     * decrement.
     */
    unsigned int exponential_counter_period;
    unsigned int new_exponential_counter_period;

    unsigned int state_pipeline;

    ///
    unsigned int envelope_pipeline;

    unsigned int exponential_pipeline;

    /// Current envelope state
    State state;
    State next_state;

    /// Whether counter is enabled. Only switching to ATTACK can release envelope.
    bool counter_enabled;

    /// Gate bit
    bool gate;

    ///
    bool resetLfsr;

    /// The current digital value of envelope output.
    unsigned char envelope_counter;

    /// Attack register
    unsigned char attack;

    /// Decay register
    unsigned char decay;

    /// Sustain register
    unsigned char sustain;

    /// Release register
    unsigned char release;

    /// The ENV3 value, sampled at the first phase of the clock
    unsigned char env3;

    /**
     * Emulated nonlinearity of the envelope DAC.
     *
     * @See Dac
     */
    float dac[256];

private:
    static const unsigned int adsrtable[16];

private:
    void set_exponential_counter();

    void state_change();

public:
    /**
     * Set chip model.
     * This determines the type of the analog DAC emulation:
     * 8580 is perfectly linear while 6581 is nonlinear.
     *
     * @param chipModel
     */
    void setChipModel(ChipModel chipModel);

    /**
     * SID clocking.
     */
    void clock();

    /**
     * Get the Envelope Generator output.
     * DAC imperfections are emulated by using envelope_counter as an index
     * into a DAC lookup table. readENV() uses envelope_counter directly.
     */
    float output() const { return dac[envelope_counter]; }

    /**
     * Constructor.
     */
    EnvelopeGenerator() :
        lfsr(0x7fff),
        rate(0),
        exponential_counter(0),
        exponential_counter_period(1),
        new_exponential_counter_period(0),
        state_pipeline(0),
        envelope_pipeline(0),
        exponential_pipeline(0),
        state(RELEASE),
        counter_enabled(true),
        gate(false),
        resetLfsr(false),
        envelope_counter(0xaa),
        attack(0),
        decay(0),
        sustain(0),
        release(0) {}

    /**
     * SID reset.
     */
    void reset();

    /**
     * Write control register.
     *
     * @param control
     *            control register
     */
    void writeCONTROL_REG(unsigned char control);

    /**
     * Write Attack/Decay register.
     *
     * @param attack_decay
     *            attack/decay value
     */
    void writeATTACK_DECAY(unsigned char attack_decay);

    /**
     * Write Sustain/Release register.
     *
     * @param sustain_release
     *            sustain/release value
     */
    void writeSUSTAIN_RELEASE(unsigned char sustain_release);

    /**
     * Return the envelope current value.
     *
     * @return envelope counter
     */
    unsigned char readENV() const { return env3; }
};

} // namespace reSIDfp

#if RESID_INLINING || defined(ENVELOPEGENERATOR_CPP)

namespace reSIDfp
{

RESID_INLINE
void EnvelopeGenerator::clock()
{
    env3 = envelope_counter;

    if (unlikely(new_exponential_counter_period > 0))
    {
        exponential_counter_period = new_exponential_counter_period;
        new_exponential_counter_period = 0;
    }

    if (unlikely(state_pipeline))
    {
        state_change();
    }

    if (unlikely(envelope_pipeline != 0) && (--envelope_pipeline == 0))
    {
        if (likely(counter_enabled))
        {
            if (state == ATTACK)
            {
                envelope_counter++;
                if (unlikely(envelope_counter == 0xff))
                {
                    next_state = DECAY_SUSTAIN;
                    state_pipeline = 3;
                }
            }
            else if ((state == DECAY_SUSTAIN) || (state == RELEASE))
            {
                envelope_counter--;
            }

            set_exponential_counter();
        }
    }

    if (unlikely(exponential_pipeline != 0) && (--exponential_pipeline == 0))
    {
        exponential_counter = 0;

       if (((state == DECAY_SUSTAIN) && (envelope_counter != sustain))
           || (state == RELEASE))
        {
            // The envelope counter can flip from 0x00 to 0xff by changing state to
            // attack, then to release. The envelope counter will then continue
            // counting down in the release state.
            // This has been verified by sampling ENV3.

            envelope_pipeline = 1;
        }
    }
    else if (unlikely(resetLfsr))
    {
        lfsr = 0x7fff;
        resetLfsr = false;

        if (state == ATTACK)
        {
            // The first envelope step in the attack state also resets the exponential
            // counter. This has been verified by sampling ENV3.
            exponential_counter = 0; // NOTE this is actually delayed one cycle, not modeled

            // The envelope counter can flip from 0xff to 0x00 by changing state to
            // release, then to attack. The envelope counter is then frozen at
            // zero; to unlock this situation the state must be changed to release,
            // then to attack. This has been verified by sampling ENV3.

            envelope_pipeline = 2;
        }
        else
        {
            if (++exponential_counter == exponential_counter_period)
                exponential_pipeline = exponential_counter_period != 1 ? 2 : 1;
        }
    }

    // ADSR delay bug.
    // If the rate counter comparison value is set below the current value of the
    // rate counter, the counter will continue counting up until it wraps around
    // to zero at 2^15 = 0x8000, and then count rate_period - 1 before the
    // envelope can constly be stepped.
    // This has been verified by sampling ENV3.

    // check to see if LFSR matches table value
    if (likely(lfsr != rate))
    {
        // it wasn't a match, clock the LFSR once
        // by performing XOR on last 2 bits
        const unsigned int feedback = ((lfsr << 14) ^ (lfsr << 13)) & 0x4000;
        lfsr = (lfsr >> 1) | feedback;
    }
    else
    {
        resetLfsr = true;
    }
}

/**
 * This is what happens on chip during state switching,
 * based on die reverse engineering and transistor level
 * emulation.
 *
 * Attack
 *
 *  0 - Gate on
 *  1 - Counting direction changes
 *      During this cycle the decay state is "accidentally" activated
 *  2 - Counter is being inverted
 *      Now the attack state is correctly activated
 *      Counter is enabled
 *  3 - Counter will be counting upward from now on
 *
 * Decay
 *
 *  0 - Counter == $ff
 *  1 - Counting direction changes
 *      The attack state is still active
 *  2 - Counter is being inverted
 *      During this cycle the decay state is activated
 *  3 - Counter will be counting downward from now on
 *
 * Release
 *
 *  0 - Gate off
 *  1 - During this cycle the release state is activated if coming from sustain/decay
 * *2 - Counter is being inverted, the release state is activated
 * *3 - Counter will be counting downward from now on
 *
 *  (* only if coming directly from Attack state)
 *
 * Freeze
 *
 *  0 - Counter == $00
 *  1 - Nothing
 *  2 - Counter is disabled
 */
RESID_INLINE
void EnvelopeGenerator::state_change()
{
    if (((next_state == ATTACK) && (state_pipeline == 3))
        || ((next_state == DECAY_SUSTAIN) && (state_pipeline == 2)))
    {
        // The decay state is "accidentally" activated also during first cycle of attack phase
        state = DECAY_SUSTAIN;
        rate = adsrtable[decay];
    }
    else if ((next_state == ATTACK) && (state_pipeline == 2))
    {
        // The attack register is correctly activated during second cycle of attack phase
        state = ATTACK;
        rate = adsrtable[attack];
        counter_enabled = true;
    }
    else if (next_state == RELEASE)
    {
        if (((state == ATTACK) && (state_pipeline == 2))
            || ((state == DECAY_SUSTAIN) && (state_pipeline == 3)))
        {
            state = RELEASE;
            rate = adsrtable[release];
        }
    }
    else if ((next_state == FREEZED) && (state_pipeline == 1))
    {
        counter_enabled = false;
    }

    state_pipeline--;
}

} // namespace reSIDfp

#endif

#endif
