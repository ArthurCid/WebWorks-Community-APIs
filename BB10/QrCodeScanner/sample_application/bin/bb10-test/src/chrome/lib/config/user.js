module.exports = {
    "version": "1.0.0",
    "id": "community.extension.test",
    "hasMultiAccess": true,
    "accessList": [
        {
            "features": [
                {
                    "id": "blackberry.app",
                    "required": true
                },
                {
                    "id": "community.qrreader",
                    "required": true
                },
                {
                    "id": "blackberry.event"
                },
                {
                    "id": "blackberry.notification"
                }
            ],
            "uri": "WIDGET_LOCAL",
            "allowSubDomain": true
        }
    ],
    "enableFlash": false,
    "autoOrientation": true,
    "autoDeferNetworkingAndJavaScript": true,
    "icon": [
        "default-icon.png"
    ],
    "author": "Tim Windsor",
    "authorURL": "http://www.blackberry.com/developers",
    "copyright": "2013",
    "authorEmail": "@timothywindsor",
    "license": "Licensed under the Apache License, Version 2.0 (the \"License\"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an \"AS IS\" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.",
    "licenseURL": "http://www.apache.org/licenses/LICENSE-2.0",
    "content": "local:///index.html",
    "foregroundSource": "index.html",
    "permissions": [
        "use_camera"
    ],
    "name": {
        "default": "QrReader"
    },
    "description": {
        "default": "QrReader"
    },
    "configXML": "config.xml",
    "debugEnabled": true
};