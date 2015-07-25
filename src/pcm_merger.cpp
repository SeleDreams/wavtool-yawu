/*
    wavtool-yawu
    Copyright (C) 2015 StarBrilliant <m13253@hotmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program.  If not,
    see <http://www.gnu.org/licenses/>.
*/

#include "pcm_merger.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <ios>
#include <iostream>
#include <libwintf8/termio.h>
#include <sndfile.h>
#include "option_manager.hpp"
#include "pcm_file.hpp"
#include "rand_round.hpp"

namespace YAWU {

struct PCMMerger::Private {
    PCMFile input_file;
    PCMFile output_file;
    int32_t sample_rate = 0;
    ssize_t prefix_samples = 0;
    ssize_t append_samples = 0;
    int64_t existing_samples = 0;
    std::vector<double> envelope;
    std::vector<double> buffer1;
    std::vector<double> buffer2;
    bool results_written = false;
};

PCMMerger::PCMMerger(OptionManager &option_manager) :
    option_manager(option_manager) {
}

PCMMerger::~PCMMerger() {
}

PCMMerger &PCMMerger::prepare() {
    try {
        p->input_file.open(option_manager.get_input_file_name(), std::ios_base::in);
        p->sample_rate = p->input_file.sample_rate();
        if(p->input_file.channels() != 1) {
            p->input_file.close();
            throw PCMFile::FileError("Must have only one channel");
        }
    } catch(PCMFile::FileError e) {
        WTF8::cerr << "Unable to open input file: " << e.what() << std::endl;
    }
    try {
        p->output_file.open(option_manager.get_output_file_name(), std::ios_base::in | std::ios_base::out, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, p->sample_rate != 0 ? p->sample_rate : 44100);
        if(p->sample_rate != 0) {
            if(p->sample_rate != p->output_file.sample_rate()) {
                WTF8::cerr << "Warning: Sample rate mismatch between input and output file" << std::endl
                           << "Sample rate: " << p->sample_rate << " != " << p->output_file.sample_rate() << std::endl;
                p->input_file.close();
            }
        } else {
            p->sample_rate = p->output_file.sample_rate();
        }
        if(p->output_file.channels() != 1) {
            p->output_file.close();
            throw PCMFile::FileError("Must have only one channel");
        }
    } catch(PCMFile::FileError e) {
        WTF8::cerr << "Unable to open output file: " << e.what() << std::endl;
        return *this;
    }
    p->prefix_samples = ssize_t(option_manager.get_overlap() * p->sample_rate);
    if(p->prefix_samples < 0) {
        WTF8::cerr << "Warning: overlap value is negative" << std::endl;
        p->prefix_samples = 0;
    }
    p->append_samples = ssize_t(RandRound()(option_manager.get_note_length() * p->sample_rate));
    if(p->append_samples < 0) {
        WTF8::cerr << "Warning: note length is negative" << std::endl;
        p->append_samples = 0;
    }
    p->existing_samples = p->output_file.frames();
    assert(p->existing_samples >= 0);
    p->envelope = std::move(std::vector<double>(p->prefix_samples + p->append_samples, 1));
    p->buffer1 = std::move(std::vector<double>(p->prefix_samples + p->append_samples, 0));
    p->buffer2 = std::move(std::vector<double>(p->prefix_samples + p->append_samples, 0));
    return *this;
}

PCMMerger &PCMMerger::fill_overlap() {
    if(p->output_file.is_open()) {
        if(p->existing_samples >= int64_t(p->prefix_samples)) {
            p->output_file.seek(p->existing_samples - int64_t(p->prefix_samples), SEEK_SET);
            p->output_file.read(p->buffer1.data(), p->prefix_samples);
        } else {
            p->output_file.seek(0, SEEK_SET);
            p->output_file.read(&p->buffer1.data()[int64_t(p->prefix_samples) - p->existing_samples], p->existing_samples);
        }
    }
    return *this;
}

PCMMerger &PCMMerger::read_new_segment() {
    if(p->input_file.is_open()) {
        ssize_t stp_samples = ssize_t(option_manager.get_stp() * p->sample_rate);
        if(stp_samples >= 0) {
            p->input_file.seek(stp_samples, SEEK_SET);
            if(p->input_file.read(p->buffer2.data(), p->buffer2.size()) < p->buffer2.size())
                WTF8::cerr << "Warning: Input file is not long enough" << std::endl;
        } else {
            auto neg_stp_samples = -stp_samples;
            if(p->input_file.read(&p->buffer2.data()[neg_stp_samples], p->buffer2.size() - neg_stp_samples) < p->buffer2.size() - neg_stp_samples)
                WTF8::cerr << "Warning: Input file is not long enough" << std::endl;
        }
    }
    return *this;
}

PCMMerger &PCMMerger::construct_envelope() {
    return *this;
}

PCMMerger &PCMMerger::mix_new_segment() {
    assert(p->buffer1.size() == p->buffer2.size());
    assert(p->buffer2.size() == p->envelope.size());
    if(p->output_file.is_open()) {
        auto old_segments = p->buffer1.data();
        auto new_segment = p->buffer2.data();
        auto envelope = p->envelope.data();
        for(size_t i = 0; i < p->buffer1.size(); i++)
            old_segments[i] += new_segment[i] * envelope[i];
    }
    return *this;
}

PCMMerger &PCMMerger::write_back() {
    if(p->output_file.is_open()) {
        if(p->existing_samples >= int64_t(p->prefix_samples)) {
            p->output_file.seek(p->existing_samples - int64_t(p->prefix_samples), SEEK_SET);
            p->output_file.write(p->buffer1.data(), p->buffer1.size());
        } else {
            p->output_file.seek(0, SEEK_SET);
            p->output_file.write(&p->buffer1.data()[int64_t(p->prefix_samples) - p->existing_samples], int64_t(p->buffer1.size()) - (int64_t(p->prefix_samples) - p->existing_samples));
        }
    }
    return *this;
}

}
