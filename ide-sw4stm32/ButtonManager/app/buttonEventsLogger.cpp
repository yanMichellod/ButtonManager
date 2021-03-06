/*
 * buttonEventsLogger.cpp
 *
 *  Created on: 24 mars 2019
 *      Author: Yan Michellod
 */

#include <app/buttonEventsLogger.h>
#include "config/trace-config.h"
#include "trace.h"
#include <stdio.h>
#include "event/events.h"
#include "event/evbuttonpressed.h"
#include "interface/buttonEventsHandlerObserver.h"
#include "xf/behavior.h"


using namespace app;

/**
 * @brief Constructor of ButtonEventsLogger
 */
ButtonEventsLogger::ButtonEventsLogger() {
	_currentState = STATE_INITIAL;
	_buttonIndex = 0;
	isShort = false;
}

/**
 * @brief Destructor of ButtonEventsLogger
 */
ButtonEventsLogger::~ButtonEventsLogger() {
}

/**
 * @brief method serving to notify that a button was short pressed
 */
void ButtonEventsLogger::onButtonShortPressed(ButtonIndex buttonIndex) {
	_buttonIndex = buttonIndex;
	isShort = true;
	GEN(evButtonPressed());
}

/**
 * @brief method serving to notify that a button was long pressed
 */
void ButtonEventsLogger::onButtonLongPressed(ButtonIndex buttonIndex) {
	_buttonIndex = buttonIndex;
	isShort = false;
	GEN(evButtonPressed());
}

/**
 * @brief processing state machine
 */
XFEventStatus ButtonEventsLogger::processEvent() {
	XFEventStatus status = XFEventStatus::Unknown;

	switch(_currentState){

	case STATE_INITIAL :
	{
		if(getCurrentEvent()->getEventType() == XFEvent::Initial){
			GEN(XFNullTransition());

			status = XFEventStatus::Consumed;

			_currentState = STATE_WAIT;
		}

		break;
	}

	case STATE_WAIT:
	{
		if(getCurrentEvent()->getEventType() == XFEvent::NullTransition){
			_currentState = STATE_WAIT;
		}

		else if(getCurrentEvent()->getEventType() == XFEvent::Event && getCurrentEvent()->getId() == evButtonPressedId ){
			GEN(XFNullTransition);

			if(isShort){
				_currentState = STATE_SHORTPRESSED;
			}
			else{
				_currentState = STATE_LONGPRESSED;
			}
			status = XFEventStatus::Consumed;
		}
		break;
	}
	case STATE_SHORTPRESSED:
	{
		if(getCurrentEvent()->getEventType() == XFEvent::NullTransition){
			Trace::out("ButtonEventsLogger : Button %d short pressed \r\n",_buttonIndex);

			_currentState = STATE_WAIT;

			status = XFEventStatus::Consumed;

			GEN(XFNullTransition);
		}
		break;
	}
	case STATE_LONGPRESSED:
	{
		if(getCurrentEvent()->getEventType() == XFEvent::NullTransition){
			GEN(XFNullTransition);

			Trace::out("ButtonEventsLogger : Button %d long pressed \r\n",_buttonIndex);

			_currentState = STATE_WAIT;

			status = XFEventStatus::Consumed;
		}
		break;
	}

	default:
		break;

	}
	return status;
}
