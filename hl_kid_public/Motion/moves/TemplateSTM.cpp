#include "moves/TemplateSTM.h"

#include "moves/Walk.h"
#include "moves/Placer.h"
#include "moves/Approach.h"

#include <services/LocalisationService.h>
#include <rhoban_geometry/point.h>

#include <rhoban_utils/logging/logger.h>

#define STATE_GO_TO_BALL "go_to_ball"
#define STATE_GO_TO_CENTER "go_to_center"

static rhoban_utils::Logger logger("TemplateSTM");


TemplateSTM::TemplateSTM(Walk *walk, Placer *placer, Approach *approach)
  : walk(walk), placer(placer), approach(approach)
{
  initializeBinding();
    // State
    bind->bindNew("state", STM::state, RhIO::Bind::PushOnly)
        ->comment("TemplateSTM state");
}

std::string TemplateSTM::getName()
{
  return "template_stm";
}

void TemplateSTM::onStart()
{
  bind->pull();
  startMove("placer");
  setState(STATE_GO_TO_BALL);
}

void TemplateSTM::onStop()
{
  stopMove("placer");
}

void TemplateSTM::step(float elapsed)
{
  bind->pull();
  auto loc = getServices()->localisation;

  if(state == STATE_GO_TO_BALL){
    if(!placer->arrived){
      auto ball = loc->getBallPosField();
      placer->goTo(ball.x, ball.y, 0);
    }
    else{
      setState(STATE_GO_TO_CENTER);
    }
  }
  if(state == STATE_GO_TO_CENTER){
    if(!placer->arrived){
      placer->goTo(0, 0, 0);
    }
    else{
      setState(STATE_GO_TO_BALL);
    }
  }
  bind->push();
}


void TemplateSTM::enterState(std::string state)
{ 
  auto loc = getServices()->localisation;

  if(state == STATE_GO_TO_BALL){
    auto ball = loc->getBallPosField();
    placer->goTo(ball.x, ball.y, 0);
    placer->updateArrived();
  }
  if(state == STATE_GO_TO_CENTER){
    placer->goTo(0,0,0);
    placer->updateArrived();
  }
}

void TemplateSTM::exitState(std::string state)
{
  if(state == STATE_GO_TO_BALL){
  }
  if(state == STATE_GO_TO_CENTER){
  }
}

