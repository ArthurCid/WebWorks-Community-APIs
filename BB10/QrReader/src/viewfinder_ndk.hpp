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

#ifndef ViewfinderNDK_HPP_
#define ViewfinderNDK_HPP_

#include <string>
#include <camera/camera_api.h>
#include <sstream>
#include <json/reader.h>
#include <json/writer.h>
#include <pthread.h>
#include "viewfinder_ndk.hpp"
#include "viewfinder_js.hpp"

class ViewfinderJS;

//namespace webworks {

class ViewfinderNDK {
public:
	explicit ViewfinderNDK(ViewfinderJS *parent = NULL);
	virtual ~ViewfinderNDK();
	int StartFrontCamera();
	int StartRearCamera();
	static ViewfinderJS *m_pParent;

private:
	//ViewfinderJS *m_pParent;
	camera_unit_t m_CameraUnit;
	camera_handle_t m_CameraHandle;
    std::string m_barcodeData;
    bool m_landscapePreviewFrames;

	int createViewfinder(camera_unit_t cameraUnit,
	                 	 const std::string &group,
	                 	 const std::string &id);

	void viewfinderCallback(camera_handle_t cameraHandle, camera_buffer_t* cbuffer, void* arg);
};

//} // namespace webworks

#endif /* ViewfinderNDK_H_ */
