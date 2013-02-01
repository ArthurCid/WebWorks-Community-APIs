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
#include <json/reader.h>
#include <json/writer.h>
#include <pthread.h>
#include "viewfinder_ndk.hpp"
#include "viewfinder_js.hpp"

#include <zxing/common/GreyscaleLuminanceSource.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/qrcode/QRCodeReader.h>

using namespace zxing;
using namespace zxing::qrcode;

//namespace webworks {

unsigned int frameCount;

void ViewfinderNDK::viewfinderCallback(camera_handle_t cameraHandle, camera_buffer_t* cbuffer, void* arg) {
	try {
		// these values need to be updated. Find out how to extract this info from the camera buffer
		int stride = 0;
		int width  = 0;
		int height = 0;

		Ref<LuminanceSource> source(new GreyscaleLuminanceSource(cbuffer->framebuf, stride,
		cbuffer->framesize / stride, 0, 0, width, height));
		Ref<Binarizer> binarizer(new HybridBinarizer(source));
		Ref<BinaryBitmap> bitmap(new BinaryBitmap(binarizer));
		Ref<Result> result;

		QRCodeReader *reader = new QRCodeReader();
		DecodeHints hints;
		hints.addFormat(BarcodeFormat_QR_CODE);
		hints.addFormat(BarcodeFormat_EAN_8);
		hints.addFormat(BarcodeFormat_EAN_13);
		hints.addFormat(BarcodeFormat_UPC_A);
		hints.addFormat(BarcodeFormat_UPC_E);
		hints.addFormat(BarcodeFormat_DATA_MATRIX);
		hints.addFormat(BarcodeFormat_CODE_128);
		hints.addFormat(BarcodeFormat_CODE_39);
		hints.addFormat(BarcodeFormat_ITF);
		hints.addFormat(BarcodeFormat_AZTEC);

		// If the preview buffer is in landscape, we can rotate out bitmap to let us scan 1D codes
		if (m_landscapePreviewFrames) {
			Ref<BinaryBitmap> rotated = bitmap->rotateCounterClockwise();
			result = reader->decode(rotated, hints);
		}
		else {
			result = reader->decode(bitmap, hints);
		}

		// get the result of decoding the image
		std::string newBarcodeData = result->getText()->getText().data();

		if (newBarcodeData != m_barcodeData) {
			m_barcodeData = newBarcodeData;
			//emit newBarcodeDetected(m_barcodeData);
		}

	} catch (std::exception &e) {
		// add error handling code here
	}

	/*std::string event = "viewfinderCallback";
	Json::FastWriter writer;
	Json::Value root;
	root["frameCount"] = frameCount++;
	//static_cast<ViewfinderJS*>(arg)->NotifyEvent
	ViewfinderNDK::m_pParent->NotifyEvent(event + " " + writer.write(root));*/
}

ViewfinderNDK::ViewfinderNDK(ViewfinderJS *parent) {
	m_CameraHandle = CAMERA_HANDLE_INVALID;
	m_pParent      = parent;
	frameCount     = 0;
	m_landscapePreviewFrames = false;
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

    if (camera_open(m_CameraUnit, CAMERA_MODE_RW, &m_CameraHandle) != CAMERA_EOK) {
        //qDebug() << "could not open camera";
        return EIO;
    }

    //qDebug() << "camera opened";

    // some of these camera properties may be unnecessary, we can probably reduce the framerate setting for QR scan purposes
    if (camera_set_photovf_property(m_CameraHandle,
    		                              CAMERA_IMGPROP_ROTATION, 90,
    		                              CAMERA_IMGPROP_BURSTMODE, 1,
    		                              CAMERA_IMGPROP_FRAMERATE, (double)15.0,
    		                              CAMERA_IMGPROP_HWOVERLAY, 0,
    		                              CAMERA_IMGPROP_ZOOMFACTOR, 20) == CAMERA_EOK) {
        //qDebug() << "viewfinder configured";

    	//need to update this call once viewfinderCallback is working
        if (camera_start_photo_viewfinder(m_CameraHandle, NULL, NULL, NULL) == CAMERA_EOK) {
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

//} /* namespace webworks */
