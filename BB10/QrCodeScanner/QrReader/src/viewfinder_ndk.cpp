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
#include <sstream>
//#include <json/reader.h>
//#include <json/writer.h>
#include <pthread.h>
#include "viewfinder_ndk.hpp"
#include "viewfinder_js.hpp"


namespace webworks {

unsigned int frameCount;

static void viewfinderCallback(camera_handle_t cameraHandle, camera_buffer_t cbuffer, void* arg) {
	std::string event = "viewfinderCallback";
	Json::FastWriter writer;
	Json::Value root;
	root["frameCount"] = frameCount++;
	m_pParent->NotifyEvent(event + " " + writer.write(root));
}

ViewfinderNDK::ViewfinderNDK(ViewfinderJS *parent) {
	m_CameraHandle = CAMERA_HANDLE_INVALID;
	m_pParent      = parent;
	frameCount     = 0;
}

ViewfinderNDK::~ViewfinderNDK() {
}

int ViewfinderNDK::createViewfinder(camera_unit_t cameraUnit,
                                     const std::string &group,
                                     const std::string &id)
{
    if (m_CameraHandle != CAMERA_HANDLE_INVALID)
    {
        //qDebug() << "camera already running";
        return EBUSY;
    }

    m_CameraUnit = cameraUnit;

    if (camera_open(m_CameraUnit,
    				CAMERA_MODE_RW,
                    &m_CameraHandle) != CAMERA_EOK) {
        //qDebug() << "could not open camera";
        return -18;
    }

    //qDebug() << "camera opened";

    if (camera_set_photovf_property(m_CameraHandle,
    									//CAMERA_IMGPROP_WIDTH, 1024,
    		                            //CAMERA_IMGPROP_HEIGHT, 768,
    		                              CAMERA_IMGPROP_ROTATION, 90,
    		                              CAMERA_IMGPROP_BURSTMODE, 1,
    		                              CAMERA_IMGPROP_FRAMERATE, (double)15.0,
    		                              CAMERA_IMGPROP_HWOVERLAY, 0,
    		                              CAMERA_IMGPROP_ZOOMFACTOR, 20) == CAMERA_EOK) {
        //qDebug() << "viewfinder configured";
        if (camera_start_photo_viewfinder(m_CameraHandle, NULL, NULL, NULL) == CAMERA_EOK) {
            //qDebug() << "viewfinder started";
        	camera_close(m_CameraHandle);
        	m_CameraHandle = CAMERA_HANDLE_INVALID;
            return EOK;
        }
    }
    else {
    	return EBUSY;
    }
    //qDebug() << "couldn't start viewfinder";
    camera_close(m_CameraHandle);
    m_CameraHandle = CAMERA_HANDLE_INVALID;
    return EIO;
}

int ViewfinderNDK::StartFrontCamera()
{
    return createViewfinder(CAMERA_UNIT_FRONT, "", "");
}


int ViewfinderNDK::StartRearCamera()
{
	return createViewfinder(CAMERA_UNIT_REAR, "", "");
}

} /* namespace webworks */
