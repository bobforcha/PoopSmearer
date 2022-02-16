/*
  ==============================================================================

    ShitClipper.cpp
    Created: 15 Feb 2022 11:06:33pm
    Author:  bob

  ==============================================================================
*/

#include "ShitClipper.h"

ShitClipper::ShitClipper()
{}

ShitClipper::~ShitClipper()
{}

void ShitClipper::prepare(juce::dsp::ProcessSpec spec)
{
  dryWet.prepare(spec);
  wetChain.prepare(spec);
}

void ShitClipper::process()
{}

