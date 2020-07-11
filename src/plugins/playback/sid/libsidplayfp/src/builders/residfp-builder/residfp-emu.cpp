/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2011-2019 Leandro Nini <drfiemost@users.sourceforge.net>
 * Copyright 2007-2010 Antti Lankila
 * Copyright 2001 Simon White
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

#include "residfp-emu.h"

#include <sstream>
#include <string>
#include <algorithm>

#include "residfp/siddefs-fp.h"
#include "sidplayfp/siddefs.h"
#include "Voice.h"

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

namespace libsidplayfp
{

const char* ReSIDfp::getCredits()
{
    static std::string credits;

    if (credits.empty())
    {
        // Setup credits
        std::ostringstream ss;
        ss << "ReSIDfp V" << "2.0beta" << " Engine:\n";
        ss << "\t(C) 1999-2002 Simon White\n";
        ss << "MOS6581 (SID) Emulation (ReSIDfp V" << "2.0" << "):\n";
        ss << "\t(C) 1999-2002 Dag Lem\n";
        ss << "\t(C) 2005-2011 Antti S. Lankila\n";
        ss << "\t(C) 2010-2019 Leandro Nini\n";
        credits = ss.str();
    }

    return credits.c_str();
}

ReSIDfp::ReSIDfp(sidbuilder *builder) :
    sidemu(builder),
    m_sid(*(new reSIDfp::SID))
{
    m_buffer = new short[OUTPUTBUFFERSIZE];
    reset(0);
}

ReSIDfp::~ReSIDfp()
{
    delete &m_sid;
    delete[] m_buffer;
}

void ReSIDfp::filter6581Curve(double filterCurve)
{
   m_sid.setFilter6581Curve(filterCurve);
}

void ReSIDfp::filter8580Curve(double filterCurve)
{
   m_sid.setFilter8580Curve(filterCurve);
}

// Standard component options
void ReSIDfp::reset(uint8_t volume)
{
    m_accessClk = 0;
    m_sid.reset();
    m_sid.write(0x18, volume);
}

uint8_t ReSIDfp::read(uint_least8_t addr)
{
    clock();
    return m_sid.read(addr);
}

void ReSIDfp::write(uint_least8_t addr, uint8_t data)
{
    clock();
    m_sid.write(addr, data);
}

struct FreqNoteEntry {
    const char* name;
    unsigned short v0;
    unsigned short v1;
};

static FreqNoteEntry s_freq_to_note[] = {
    { "C-0", 0x0115, 0x0112	},
    { "C#0", 0x0125, 0x0122	},
    { "D-0", 0x0137, 0x0133	},
    { "D#0", 0x0149, 0x0146	},
    { "E-0", 0x015D, 0x0159	},
    { "F-0", 0x0172, 0x016E	},
    { "F#0", 0x0188, 0x0183	},
    { "G-0", 0x019F, 0x019A	},
    { "G#0", 0x01B8, 0x01B3	},
    { "A-0", 0x01D2, 0x01CD	},
    { "A#0", 0x01EE, 0x01E8	},
    { "B-0", 0x020B, 0x0205	},

    { "C-1", 0x022A, 0x0224	},
    { "C#1", 0x024B, 0x0244	},
    { "D-1", 0x026E, 0x0267	},
    { "D#1", 0x0293, 0x028B	},
    { "E-1", 0x02BA, 0x02B2	},
    { "F-1", 0x02E3, 0x02DB	},
    { "F#1", 0x030F, 0x0307	},
    { "G-1", 0x033E, 0x0335	},
    { "G#1", 0x036F, 0x0366	},
    { "A-1", 0x03A4, 0x0399	},
    { "A#1", 0x03DB, 0x03D0	},
    { "B-1", 0x0416, 0x040A	},

    { "C-2", 0x0454, 0x0448	},
    { "C#2", 0x0496, 0x0489	},
    { "D-2", 0x04DC, 0x04CE	},
    { "D#2", 0x0526, 0x0517	},
    { "E-2", 0x0574, 0x0564	},
    { "F-2", 0x05C7, 0x05B6	},
    { "F#2", 0x061F, 0x060D	},
    { "G-2", 0x067C, 0x066A	},
    { "G#2", 0x06DF, 0x06CB	},
    { "A-2", 0x0747, 0x0733	},
    { "A#2", 0x07B6, 0x07A0	},
    { "B-2", 0x082C, 0x0814	},

    { "C-3", 0x08A8, 0x088F	},
    { "C#3", 0x092C, 0x0911	},
    { "D-3", 0x09B7, 0x099B	},
    { "D#3", 0x0A4B, 0x0A2E	},
    { "E-3", 0x0AE8, 0x0AC9	},
    { "F-3", 0x0B8E, 0x0B6D	},
    { "F#3", 0x0C3E, 0x0C1B	},
    { "G-3", 0x0CF8, 0x0CD3	},
    { "G#3", 0x0DBE, 0x0D96	},
    { "A-3", 0x0E8F, 0x0E65	},
    { "A#3", 0x0F6C, 0x0F40	},
    { "B-3", 0x1057, 0x1028	},

    { "C-4", 0x1150, 0x111E },
    { "C#4", 0x1258, 0x1223 },
    { "D-4", 0x136F, 0x1337 },
    { "D#4", 0x1496, 0x145B },
    { "E-4", 0x15D0, 0x1591 },
    { "F-4", 0x171C, 0x16DA },
    { "F#4", 0x187C, 0x1836 },
    { "G-4", 0x19F0, 0x19A6 },
    { "G#4", 0x1B7B, 0x1B2C },
    { "A-4", 0x1D1E, 0x1CCA },
    { "A#4", 0x1ED9, 0x1E80 },
    { "B-4", 0x20AE, 0x2050 },

    { "C-5", 0x22A0, 0x223C },
    { "C#5", 0x24AF, 0x2446 },
    { "D-5", 0x26DD, 0x266E },
    { "D#5", 0x292D, 0x28B7 },
    { "E-5", 0x2BA0, 0x2B22 },
    { "F-5", 0x2E38, 0x2DB3 },
    { "F#5", 0x30F8, 0x306B },
    { "G-5", 0x33E1, 0x334C },
    { "G#5", 0x36F6, 0x3659 },
    { "A-5", 0x3A3B, 0x3994 },
    { "A#5", 0x3DB2, 0x3D00 },
    { "B-5", 0x415C, 0x40A1 },

    { "C-6", 0x4540, 0x4479 },
    { "C#6", 0x495E, 0x488C },
    { "D-6", 0x4DBB, 0x4CD3 },
    { "D#6", 0x525A, 0x516D },
    { "E-6", 0x573F, 0x5645 },
    { "F-6", 0x5C6F, 0x5B66 },
    { "F#6", 0x61EF, 0x40D6 },
    { "G-6", 0x67C2, 0x6698 },
    { "G#6", 0x6DED, 0x6CB1 },
    { "A-6", 0x7476, 0x7328 },
    { "A#6", 0x7B63, 0x7A01 },
    { "H-6", 0x82B9, 0x8142 },

    { "C-7", 0x8A7F, 0x88F2 },
    { "C#7", 0x92BC, 0x9117 },
    { "D-7", 0x9B75, 0x99B7 },
    { "D#7", 0xA4B4, 0xA2DB },
    { "E-7", 0xAE7F, 0xAC8A },
    { "F-7", 0xB8DF, 0xB6CC },
    { "F#7", 0xC3DE, 0xC1AC },
    { "G-7", 0xCF84, 0xCD30 },
    { "G#7", 0xDBD9, 0xD962 },
    { "A-7", 0xE8ED, 0xE650 },
    { "A#7", 0xF6C6, 0xF402 },
};

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))

// TODO: this is really slow, we should do a a binary search or hashmap instead

const char* get_note_from_freq(unsigned int freq) {
    for (int i = 0; i < sizeof_array(s_freq_to_note); ++i) {
        if (freq > s_freq_to_note[i].v0)
            continue;

        return s_freq_to_note[i].name;
    }

    return "B-7";
}

void ReSIDfp::clock()
{
    const event_clock_t cycles = eventScheduler->getTime(m_accessClk, EVENT_CLOCK_PHI1);
    m_accessClk += cycles;
    m_bufferpos += m_sid.clock(cycles, m_buffer+m_bufferpos);

    /*
    for (int i = 0; i < 3; ++i) {
        unsigned int freq = m_sid.voice[i].get()->wave()->freq;
        printf("%s ", get_note_from_freq(freq));
    }
    printf("\n");
    */

    //double clock = double(m_accessClk);
    //double PAL = 985248.0;
    //printf("clock %f\n", clock / PAL);
}

void ReSIDfp::filter(bool enable)
{
      m_sid.enableFilter(enable);
}

void ReSIDfp::sampling(float systemclock, float freq,
        SidConfig::sampling_method_t method, bool)
{
    reSIDfp::SamplingMethod sampleMethod;
    switch (method)
    {
    case SidConfig::INTERPOLATE:
        sampleMethod = reSIDfp::DECIMATE;
        break;
    case SidConfig::RESAMPLE_INTERPOLATE:
        sampleMethod = reSIDfp::RESAMPLE;
        break;
    default:
        m_status = false;
        m_error = ERR_INVALID_SAMPLING;
        return;
    }

    try
    {
        // Round half frequency to the nearest multiple of 5000
        const int halfFreq = 5000*((static_cast<int>(freq)+5000)/10000);
        m_sid.setSamplingParameters(systemclock, sampleMethod, freq, std::min(halfFreq, 20000));
    }
    catch (reSIDfp::SIDError const &)
    {
        m_status = false;
        m_error = ERR_UNSUPPORTED_FREQ;
        return;
    }

    m_status = true;
}

// Set the emulated SID model
void ReSIDfp::model(SidConfig::sid_model_t model, bool digiboost)
{
    reSIDfp::ChipModel chipModel;
    switch (model)
    {
        case SidConfig::MOS6581:
            chipModel = reSIDfp::MOS6581;
            break;
        case SidConfig::MOS8580:
            chipModel = reSIDfp::MOS8580;
            if (digiboost)
                m_sid.input(-32768);
            break;
        default:
            m_status = false;
            m_error = ERR_INVALID_CHIP;
            return;
    }

    m_sid.setChipModel(chipModel);
    m_status = true;
}

}
