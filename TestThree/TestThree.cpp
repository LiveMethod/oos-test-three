#include "TestThree.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "IPlugPaths.h"
#include "IPlugProcessor.h"

TestThree::TestThree(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets)), delayedSamples(mSampleRate) {
  GetParam(kParamActive)->InitBool("active", true);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    const IBitmap backgroundSprite = pGraphics->LoadBitmap((BACKGROUND_FN), 2, true);

    pGraphics->EnableMouseOver(false);
    pGraphics->EnableMultiTouch(false);
    pGraphics->EnableTooltips(false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    
      IRECT b = pGraphics->GetBounds();//.GetPadded(-5);
    
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    
    // A control style that suppresses labels and values, as well
    // as making all fills transparent. These invisible buttons work
    // like hit areas over other graphics.
    const IVStyle style {
      false, // Show label
      false, // Show value
      {
        COLOR_TRANSPARENT, // Background
        COLOR_TRANSPARENT, // Foreground
        COLOR_TRANSPARENT, // Pressed
        COLOR_TRANSPARENT, // Frame
        COLOR_TRANSPARENT, // Highlight
        COLOR_TRANSPARENT, // Shadow
        COLOR_TRANSPARENT, // Extra 1
        COLOR_TRANSPARENT, // Extra 2
        COLOR_TRANSPARENT  // Extra 3
      }, // Colors
      IText(12.f, EAlign::Center) // Label text
    };
    
    const IText robotoText {16.f, "Roboto-Regular"};

    const int nRows = 1;
    const int nCols = 1;
    
    int cellIdx = 0;

    auto sameCell = [&](){
      return b.GetGridCell(cellIdx, nRows, nCols);
    };
      
    
//      The way this works is a bit stupid, but it does work...
//      The entire background is one clickable toggle with the full image.
//      Atop that are placed a bunch of invisible buttons that do nothing
//      except stop event propagation, so the background can only be clicked
//      through the area not covered by invisible objects.

    pGraphics->AttachControl(
                             new IBSwitchControl(0, 0, backgroundSprite, kParamActive),
                             kNoTag,
                             "arbitraryGroupName"
    );
      
      // A button that is blank when unchecked, and shows a checkmark when checked
      // It still relies on the forkAwesome font but I don't feel like disentangling that so we'll ship it like this lol
    pGraphics->AttachControl(new IVToggleControl(sameCell().SubRectVertical(2, 0), kNoTag, "InvisibleIVToggleControlTargetArea", style.WithValueText(robotoText), "", ""), kNoTag, "vcontrols"); // 0-50% VH
    pGraphics->AttachControl(new IVToggleControl(sameCell().SubRectVertical(4, 2).GetVShifted(-12), kNoTag, "InvisibleIVToggleControlTargetArea", style.WithValueText(robotoText), "", ""), kNoTag, "vcontrols"); // 50-75% VH
    pGraphics->AttachControl(new IVToggleControl(sameCell().SubRectVertical(8, 7).GetVShifted(3), kNoTag, "InvisibleIVToggleControlTargetArea", style.WithValueText(robotoText), "", ""), kNoTag, "vcontrols"); // The last bit of the bottom
    pGraphics->AttachControl(new IVToggleControl(sameCell().SubRectHorizontal(6,0), kNoTag, "InvisibleIVToggleControlTargetArea", style.WithValueText(robotoText), "", ""), kNoTag, "vcontrols"); // Left edge
    pGraphics->AttachControl(new IVToggleControl(sameCell().SubRectHorizontal(6,5), kNoTag, "InvisibleIVToggleControlTargetArea", style.WithValueText(robotoText), "", ""), kNoTag, "vcontrols"); // Right edge

    const IRECT bounds = pGraphics->GetBounds();
    const IRECT innerBounds = bounds.GetPadded(-3.f);
    const IRECT versionBounds = innerBounds.GetFromBRHC(300, 0);
    WDL_String buildInfoStr;
    GetBuildInfoStr(buildInfoStr, __DATE__, __TIME__);
    pGraphics->AttachControl(new ITextControl(versionBounds, buildInfoStr.Get(), DEFAULT_TEXT.WithAlign(EAlign::Far)), kCtrlTagVersionNumber);\

    // ==========
    // BEGIN OOS EXAMPLE LAYOUT
    // ==========
    // const IRECT bounds = pGraphics->GetBounds();
    // const IRECT innerBounds = bounds.GetPadded(-10.f);
    // const IRECT sliderBounds = innerBounds.GetFromLeft(150).GetMidVPadded(100);
    // const IRECT versionBounds = innerBounds.GetFromTRHC(300, 20);
    // const IRECT titleBounds = innerBounds.GetCentredInside(200, 50);

    // if (pGraphics->NControls()) {
    //   pGraphics->GetBackgroundControl()->SetTargetAndDrawRECTs(bounds);
    //   pGraphics->GetControlWithTag(kCtrlTagSlider)->SetTargetAndDrawRECTs(sliderBounds);
    //   pGraphics->GetControlWithTag(kCtrlTagTitle)->SetTargetAndDrawRECTs(titleBounds);
    //   pGraphics->GetControlWithTag(kCtrlTagVersionNumber)->SetTargetAndDrawRECTs(versionBounds);
    //   return;
    // }

    // pGraphics->SetLayoutOnResize(true);
    // pGraphics->AttachCornerResizer(EUIResizerMode::Size, true);
    // pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    // pGraphics->AttachPanelBackground(COLOR_RED);
    // pGraphics->AttachControl(new IVSliderControl(sliderBounds, kParamGain), kCtrlTagSlider);
    // pGraphics->AttachControl(new ITextControl(titleBounds, "TestThree", IText(30)), kCtrlTagTitle);
    // WDL_String buildInfoStr;
    // GetBuildInfoStr(buildInfoStr, __DATE__, __TIME__);
    // pGraphics->AttachControl(new ITextControl(versionBounds, buildInfoStr.Get(), DEFAULT_TEXT.WithAlign(EAlign::Far)), kCtrlTagVersionNumber);
    // ==========
    // END OOS EXAMPLE LAYOUT
    // ==========
  };
#endif
}

#if IPLUG_EDITOR
void TestThree::OnParentWindowResize(int width, int height)
{
  if(GetUI())
    GetUI()->Resize(width, height, 1.f, false);
}
#endif

#if IPLUG_DSP

void TestThree::OnReset()
{
  if (GetSampleRate() != mSampleRate)
  {
    mSampleRate = GetSampleRate();
    delayedSamples.resetWithSize(mSampleRate);
  }
}

void TestThree::ProcessBlock(sample** inputs, sample** outputs, int nFrames) { 
  const double sampleRate = GetSampleRate();
  const unsigned int sampleDelay = (unsigned int)((sampleRate / 1000.0) * 27);
  for (int sampleIndex = 0; sampleIndex < nFrames; sampleIndex++) {
    sample currentMonoInput = inputs[0][sampleIndex];
    delayedSamples.push(currentMonoInput);
    double newSample = 0.;

    while (delayedSamples.size() > 0 && delayedSamples.size() > sampleDelay){
        newSample = delayedSamples.pop();
    }
    
    // Use delayed right channel when active
    if (GetParam(kParamActive)->Value() == true) {
        outputs[0][sampleIndex] = currentMonoInput;
        outputs[1][sampleIndex] = newSample;
        
    }
    // Mirror left channel unmodified when inactive
    else {
        outputs[0][sampleIndex] = currentMonoInput;
        outputs[1][sampleIndex] = currentMonoInput;
    }
  }
}
#endif
