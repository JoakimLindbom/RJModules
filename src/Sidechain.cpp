#include "RJModules.hpp"
#include <iostream>
#include <cmath>

struct Sidechain: Module {
    enum ParamIds {
        RATIO_PARAM,
        DECAY_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CH1_INPUT,
        TRIGGER_INPUT,
        RATIO_CV_INPUT,
        DECAY_CV_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        CH1_OUTPUT,
        NUM_OUTPUTS
    };

    float decayAmount = 0.0;
    Sidechain() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
    void step() override;
};


#define ROUND(f) ((float)((f > 0.0) ? floor(f + 0.5) : ceil(f - 0.5)))

void Sidechain::step() {
    // float combined_input_1 = params[CH1_PARAM].value * clampf(inputs[CH1_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
    // float combined_input_2 = params[CH2_PARAM].value * clampf(inputs[CH2_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
    // float combined_input_3 = params[CH3_PARAM].value * clampf(inputs[CH3_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);

    // // new_value = ( (old_value - old_min) / (old_max - old_min) ) * (new_max - new_min) + new_min
    // float mapped_input_1 = ((combined_input_1 - 0.0) / (1.0 - 0.0) ) * (12.0 - -12.0) + -12.0;
    // float mapped_input_2 = ((combined_input_2 - 0.0) / (1.0 - 0.0) ) * (12.0 - -12.0) + -12.0;
    // float mapped_input_3 = ((combined_input_3 - 0.0) / (1.0 - 0.0) ) * (12.0 - -12.0) + -12.0;

    // int cast_input_1 = static_cast<int>(mapped_input_1);
    // int cast_input_2 = static_cast<int>(mapped_input_2);
    // int cast_input_3 = static_cast<int>(mapped_input_3);


    float ratio = params[RATIO_PARAM].value * clampf(inputs[RATIO_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
    float decay = 1 - (params[DECAY_PARAM].value * clampf(inputs[DECAY_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0)) + .00001;
    float decayLambda = .0001;

    if(inputs[TRIGGER_INPUT].value > 0 || inputs[TRIGGER_INPUT].value > 0){
        decayAmount = clampf(inputs[RATIO_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
    }

    outputs[CH1_OUTPUT].value = inputs[CH1_OUTPUT].value * (1 - (ratio * decayAmount));
    decayAmount = (decayAmount - (decayLambda * (decay)));
    if(decayAmount < 0){
        decayAmount = 0;
    }

}


SidechainWidget::SidechainWidget() {
    Sidechain *module = new Sidechain();
    setModule(module);
    box.size = Vec(15*10, 380);

    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/Sidechain.svg")));
        addChild(panel);
    }

    addChild(createScrew<ScrewSilver>(Vec(15, 0)));
    addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 0)));
    addChild(createScrew<ScrewSilver>(Vec(15, 365)));
    addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 365)));

    addParam(createParam<RoundBlackKnob>(Vec(57, 159), module, Sidechain::RATIO_PARAM, 0.0, 1.0, 1.0));
    addParam(createParam<RoundBlackKnob>(Vec(57, 239), module, Sidechain::DECAY_PARAM, 0.0, 1.0, 0.3));

    addInput(createInput<PJ301MPort>(Vec(22, 100), module, Sidechain::CH1_INPUT));
    addInput(createInput<PJ301MPort>(Vec(22, 180), module, Sidechain::RATIO_CV_INPUT));
    addInput(createInput<PJ301MPort>(Vec(22, 260), module, Sidechain::DECAY_CV_INPUT));
    addInput(createInput<PJ301MPort>(Vec(110, 100), module, Sidechain::TRIGGER_INPUT));

    addOutput(createOutput<PJ301MPort>(Vec(110, 305), module, Sidechain::CH1_OUTPUT));
}
