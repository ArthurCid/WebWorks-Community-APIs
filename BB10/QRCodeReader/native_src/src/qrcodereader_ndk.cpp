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

#include <json/reader.h>
#include <json/writer.h>

#include <zxing/common/GreyscaleLuminanceSource.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/MultiFormatReader.h>

#include <base64/base64.h>

#include "qrcodereader_ndk.hpp"
#include "qrcodereader_js.hpp"

#include <sstream>

using namespace zxing;
using namespace zxing::qrcode;

namespace webworks {

    /*
     * getCameraErrorDesc
     *
     * Returns a descriptive error message for a given camera error code
     */
    const char* getCameraErrorDesc(camera_error_t err) {
        char* ret;
        switch (err) {
        case CAMERA_EAGAIN:
            return "The specified camera was not available. Try again.";
        case CAMERA_EINVAL:
            return "The camera call failed because of an invalid parameter.";
        case CAMERA_ENODEV:
            return "No such camera was found.";
        case CAMERA_EMFILE:
            return "The camera called failed because of a file table overflow.";
        case CAMERA_EBADF:
            return "Indicates that an invalid handle to a @c camera_handle_t value was used.";
        case CAMERA_EACCESS:
            return "Indicates that the necessary permissions to access the camera are not available.";
        case CAMERA_EBADR:
            return "Indicates that an invalid file descriptor was used.";
        case CAMERA_ENOENT:
            return "Indicates that the access a file or directory that does not exist.";
        case CAMERA_ENOMEM:
            return "Indicates that memory allocation failed.";
        case CAMERA_EOPNOTSUPP:
            return "Indicates that the requested operation is not supported.";
        case CAMERA_ETIMEDOUT:
            return "Indicates an operation on the camera is already in progress. In addition, this error can indicate that an error could not be completed because i was already completed. For example, if you called the @c camera_stop_video() function but the camera had already stopped recording video, this error code would be returned.";
        case CAMERA_EALREADY:
            return "Indicates an operation on the camera is already in progress. In addition,this error can indicate that an error could not be completed because it was already completed. For example, if you called the @c camera_stop_video() function but the camera had already stopped recording video, this error code would be returned.";
        case CAMERA_EUNINIT:
            return "Indicates that the Camera Library is not initialized.";
        case CAMERA_EREGFAULT:
            return "Indicates that registration of a callback failed.";
        case CAMERA_EMICINUSE:
            return "Indicates that it failed to open because microphone is already in use.";
        }
        return NULL;
    }


    /*
     * viewfinder_callback
     *
     * This callback is invoked when an image frame from the camera viewfinder becomes available.
     * The frame is analyzed to determine if a QR code can be matched.
     */
    void viewfinder_callback(camera_handle_t handle,camera_buffer_t* buf,void* arg) {
        camera_frame_rgb888_t* data = (camera_frame_rgb888_t*)(&(buf->framedesc));
        uint8_t* buff = buf->framebuf;
        int stride = data->stride;
        int width = data->width;
        int height = data->height;

        std::string bitmapHeader = getBMPHeader(width, height);

        Json::FastWriter writer;
        Json::Value root;
        root["header"] = base64_encode((const unsigned char *)bitmapHeader.c_str(), bitmapHeader.length());
        root["frame"]  = base64_encodeRGB888((const unsigned char *)buff, width * height);
        std::string event = "community.QRCodeReader.codeFoundCallback.native";

        // push encoded frame back to caller
        QRCodeReaderNDK::m_pParent->NotifyEvent(event + " " + writer.write(root));

        try{
            Ref<LuminanceSource> source(new GreyscaleLuminanceSource((unsigned char *)buff, stride, height, 0,0,width,height));

            Ref<Binarizer> binarizer(new HybridBinarizer(source));
            Ref<BinaryBitmap> bitmap(new BinaryBitmap(binarizer));
            Ref<Result> result;

            // setup the QR code reader
            QRCodeReader *reader = new QRCodeReader();
            DecodeHints *hints = new DecodeHints();

            hints->addFormat(BarcodeFormat_QR_CODE);
            hints->addFormat(BarcodeFormat_EAN_8);
            hints->addFormat(BarcodeFormat_EAN_13);
            hints->addFormat(BarcodeFormat_UPC_A);
            hints->addFormat(BarcodeFormat_UPC_E);
            hints->addFormat(BarcodeFormat_DATA_MATRIX);
            hints->addFormat(BarcodeFormat_CODE_128);
            hints->addFormat(BarcodeFormat_CODE_39);
            hints->addFormat(BarcodeFormat_ITF);
            hints->addFormat(BarcodeFormat_AZTEC);

            // attempt to decode and retrieve a valid QR code from the image bitmap
            result = reader->decode(bitmap, *hints);
            std::string newBarcodeData = result->getText()->getText().data();

            Json::FastWriter writer;
            Json::Value root;
            root["successful"] = true;
            root["error"] = EOK;
            root["reason"] = "";
            root["value"] = newBarcodeData;
            std::string event = "community.QRCodeReader.codeFoundCallback.native";

            // notify caller that a valid QR code has been decoded
            QRCodeReaderNDK::m_pParent->NotifyEvent(event + " " + writer.write(root));
            QRCodeReaderNDK::stopQRCodeRead();

#ifdef DEBUG
            fprintf(stderr, "This is the detected QR Code : %s\n", newBarcodeData.c_str());
#endif
        }
        catch (const std::exception& ex)
        {
#ifdef DEBUG
            fprintf( stderr, "error occured : \%s \n", ex.what());
#endif
        }
    }


    /*
     * Constructor for QR reader NDK class
     */
    QRCodeReaderNDK::QRCodeReaderNDK(QRCodeReaderJS *parent) {
        m_pParent     = parent;
        mCameraHandle = CAMERA_HANDLE_INVALID;
    }

    QRCodeReaderNDK::~QRCodeReaderNDK() {}


    /*
     * QRCodeReaderNDK::startQRCodeRead
     *
     * This method is called to start a QR code read. A connection is opened to the device camera
     * and the photo viewfinder is started.
     */
    int QRCodeReaderNDK::startQRCodeRead() {

        std::string event = "community.QRCodeReader.codeFoundCallback.native";
        Json::FastWriter writer;
        Json::Value root;

        camera_error_t err;

        err = camera_open(CAMERA_UNIT_REAR,CAMERA_MODE_RW | CAMERA_MODE_ROLL,&mCameraHandle);
        if ( err != CAMERA_EOK){
#ifdef DEBUG
            fprintf(stderr, " Ran into an issue when initializing the camera = %d\n ", err);
#endif
            root["successful"] = false;
            root["error"] = err;
            root["reason"] = getCameraErrorDesc( err );
            m_pParent->NotifyEvent(event + " " + writer.write(root));
            return EIO;
        }

        err = camera_set_videovf_property(mCameraHandle,
           CAMERA_IMGPROP_FORMAT, CAMERA_FRAMETYPE_RGB8888, // <--- what we want
           CAMERA_IMGPROP_CREATEWINDOW, 0);
        if ( err != CAMERA_EOK){
#ifdef DEBUG
            fprintf(stderr, " Ran into an issue when configuring the camera = %d\n ", err);
#endif
            root["successful"] = false;
            root["error"] = err;
            root["reason"] = getCameraErrorDesc( err );
            m_pParent->NotifyEvent(event + " " + writer.write(root));
            return EIO;
        }

        err = camera_start_video_viewfinder( mCameraHandle, &viewfinder_callback, NULL, NULL);

        if ( err != CAMERA_EOK) {
#ifdef DEBUG
            fprintf(stderr, "Ran into a strange issue when starting up the camera viewfinder\n");
#endif
            root["successful"] = false;
            root["error"] = err;
            root["reason"] = getCameraErrorDesc( err );
            m_pParent->NotifyEvent(event + " " + writer.write(root));
            return EIO;
        }
        return EOK;
    }


    /*
     * QRCodeReaderNDK::stopQRCodeRead
     *
     * This method is called to clean up following successful detection of a QR code.
     * Calling this method will stop the viewfinder and close an open connection to the device camera.
     */
    int QRCodeReaderNDK::stopQRCodeRead() {
        camera_error_t err;
        Json::FastWriter writer;
        Json::Value root;
        std::string event = "community.QRCodeReader.disabledCallback.native";
        root["disabled"] = "true";

        err = camera_stop_video_viewfinder(mCameraHandle);
        if ( err != CAMERA_EOK)
        {
#ifdef DEBUG
            fprintf(stderr, "Error with turning off the photo viewfinder \n");
#endif
            root["successful"] = false;
            root["error"] = err;
            root["reason"] = getCameraErrorDesc( err );
            m_pParent->NotifyEvent(event + " " + writer.write(root));
            return EIO;
        }

        //check to see if the camera is open, if it is open, then close it
        err = camera_close(mCameraHandle);
        if ( err != CAMERA_EOK){
#ifdef DEBUG
            fprintf(stderr, "Error with closing the camera \n");
#endif
            root["successful"] = false;
            root["error"] = err;
            root["reason"] = getCameraErrorDesc( err );
            m_pParent->NotifyEvent(event + " " + writer.write(root));
            return EIO;
        }

        root["successful"] = true;
        root["error"] = EOK;
        root["reason"] = "";
        mCameraHandle = CAMERA_HANDLE_INVALID;
        m_pParent->NotifyEvent(event + " " + writer.write(root));
        return EOK;
    }

    std::string getBMPHeader(const unsigned int width, const unsigned int height) {

    	std::stringstream ss (std::stringstream::in);
    	ss << hex << (width * height);
    	std::string numFileBytes = ss.str();

        ss.str("");
        ss.clear();
    	ss << hex << (width);
    	std::string w = ss.str();

        ss.str("");
        ss.clear();
    	ss << hex << (height);
    	std::string h = ss.str();

    	return "BM" +                    // Signature
    		numFileBytes +            // size of the file (bytes)*
    	    "\x00\x00" +              // reserved
    	    "\x00\x00" +              // reserved
    	    "\x36\x00\x00\x00" +      // offset of where BMP data lives (54 bytes)
    	    "\x28\x00\x00\x00" +      // number of remaining bytes in header from here (40 bytes)
    	    w +                       // the width of the bitmap in pixels*
    	    h +                       // the height of the bitmap in pixels*
    	    "\x01\x00" +              // the number of color planes (1)
    	    "\x20\x00" +              // 32 bits / pixel
    	    "\x00\x00\x00\x00" +      // No compression (0)
    	    "\x00\x00\x00\x00" +      // size of the BMP data (bytes)*
    	    "\x13\x0B\x00\x00" +      // 2835 pixels/meter - horizontal resolution
    	    "\x13\x0B\x00\x00" +      // 2835 pixels/meter - the vertical resolution
    	    "\x00\x00\x00\x00" +      // Number of colors in the palette (keep 0 for 32-bit)
    	    "\x00\x00\x00\x00";       // 0 important colors (means all colors are important)
    }
}
