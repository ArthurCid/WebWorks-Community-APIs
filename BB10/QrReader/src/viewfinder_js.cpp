/*
 * Copyright 2013 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>
#include "../public/tokenizer.h"
#include "viewfinder_js.hpp"
#include "viewfinder_ndk.hpp"


using namespace std;

/**
 * Default constructor.
 */
ViewfinderJS::ViewfinderJS(const std::string& id) :
		m_id(id) {
	m_pTemplateController = new ViewfinderNDK(this);
}

/**
 * ViewfinderJS destructor.
 */
ViewfinderJS::~ViewfinderJS() {
	if (m_pTemplateController)
		delete m_pTemplateController;
}

/**
 * This method returns the list of objects implemented by this native
 * extension.
 */
char* onGetObjList() {
	static char name[] = "ViewfinderJS";
	return name;
}

/**
 * This method is used by JNext to instantiate the ViewfinderJS object when
 * an object is created on the JavaScript server side.
 */
JSExt* onCreateObject(const string& className, const string& id) {
	if (className == "ViewfinderJS") {
		return new ViewfinderJS(id);
	}

	return NULL;
}

/**
 * Method used by JNext to determine if the object can be deleted.
 */
bool ViewfinderJS::CanDelete() {
	return true;
}

/**
 * It will be called from JNext JavaScript side with passed string.
 * This method implements the interface for the JavaScript to native binding
 * for invoking native code. This method is triggered when JNext.invoke is
 * called on the JavaScript side with this native objects id.
 */
//TODO: might need to change return type
std::string ViewfinderJS::InvokeMethod(const string& command) {
	// command appears with parameters following after a space
	int index = command.find_first_of(" ");
	std::string strCommand = command.substr(0, index);
	std::string args = command.substr(index + 1, command.length());

	/*index = args.find_first_of(" ");
	std::string arg1 = args.substr(strCommand.length(), index);
	std::string arg2 = args.substr(index + 1, command.length());*/

	// based on the command given, run the appropriate method in template_ndk.cpp
	if (strCommand == "frontCamera") {
		//TODO: The parameters need to be updated here
		int result = m_pTemplateController->StartFrontCamera();
		if (result == EOK) {
			return std::string("OK");
		}
		else if (result == EBUSY) {
					return std::string("BUSY");
				}
		else if (result == EIO) {
			return std::string("EIO");
		}
		else {
			return std::string("ERROR");
		}
	}
	else if (strCommand == "rearCamera") {
		//TODO: The parameters need to be updated here
		int result = m_pTemplateController->StartRearCamera();
		if (result == EOK) {
			return std::string("OK");
		}
		else if (result == EBUSY) {
					return std::string("BUSY");
				}
		else if (result == EIO) {
			return std::string("EIO");
		}
		else {
			return std::string("ERROR");
		}
	}

	strCommand.append(";");
	strCommand.append(command);
	return strCommand;
}

// Notifies JavaScript of an event
void ViewfinderJS::NotifyEvent(const std::string& event) {
	std::string eventString = m_id + " ";
	eventString.append(event);
	SendPluginEvent(eventString.c_str(), m_pContext);
}
