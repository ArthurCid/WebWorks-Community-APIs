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

///////////////////////////////////////////////////////////////////
// JavaScript wrapper for JNEXT plugin for connection
///////////////////////////////////////////////////////////////////

var qrreader,
    _event = require("../../lib/event");

JNEXT.QrReader = function () {
    var self = this,
        hasInstance = false;

    self.getId = function () {
        return self.m_id;
    };

    self.init = function () {
        if (!JNEXT.require("libQrReader")) {
            return false;
        }

        self.m_id = JNEXT.createObject("libQrReader.ViewfinderJS");

        if (self.m_id === "") {
            return false;
        }

        JNEXT.registerEvents(self);
    };

    // ************************
    // Enter your methods here
    // ************************

    // calls into InvokeMethod(string command) in template_js.cpp
    self.testFront = function () {
        return JNEXT.invoke(self.m_id, "frontCamera");
    };
    self.testRear = function (input) {
        return JNEXT.invoke(self.m_id, "rearCamera");
    };

    // ************************
    // End of methods to edit
    // ************************
    self.m_id = "";

    self.getInstance = function () {
        if (!hasInstance) {
            hasInstance = true;
            self.init();
        }
        return self;
    };

};

qrreader = new JNEXT.QrReader();

module.exports = {
    qrreader: qrreader
};
