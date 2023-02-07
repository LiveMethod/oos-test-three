#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "IPlugPaths.h"
#include "SampleBuffer.h"

const int kNumPresets = 0;

enum EParams
{
  kParamActive,
  kNumParams
};

enum ECtrlTags
{
  kCtrlTagVersionNumber = 0,
  kCtrlTagSlider,
  kCtrlTagTitle
};

using namespace iplug;
using namespace igraphics;

class TestThree final : public Plugin
{
public:
  TestThree(const InstanceInfo& info);

#if IPLUG_EDITOR
  void OnParentWindowResize(int width, int height) override;
  bool OnHostRequestingSupportedViewConfiguration(int width, int height) override { return true; }
#endif
  
#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
#endif
private:
  SampleBuffer<double> delayedSamples;
  double mSampleRate = 0.0;
};
