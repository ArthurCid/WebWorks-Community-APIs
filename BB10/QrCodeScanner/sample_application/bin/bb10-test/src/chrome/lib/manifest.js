module.exports = {
    "app": {
        "global": false,
        "namespace": "blackberry.app",
        "dependencies": []
    },
    "bbm.platform": {
        "global": false,
        "namespace": "blackberry.bbm.platform",
        "dependencies": [
            "utils"
        ]
    },
    "card": {
        "global": false,
        "namespace": "blackberry.invoke.card",
        "dependencies": []
    },
    "community.qrreader.template": {
        "global": false,
        "namespace": "community.qrreader",
        "dependencies": [
            "utils"
        ]
    },
    "community.template": {
        "global": false,
        "namespace": "community.template",
        "dependencies": [
            "utils"
        ]
    },
    "connection": {
        "global": false,
        "namespace": "blackberry.connection",
        "dependencies": [
            "jpps"
        ]
    },
    "event": {
        "global": true,
        "namespace": "blackberry.event",
        "dependencies": []
    },
    "identity": {
        "global": false,
        "namespace": "blackberry.identity",
        "dependencies": []
    },
    "invoke": {
        "global": false,
        "namespace": "blackberry.invoke",
        "dependencies": []
    },
    "invoked": {
        "global": false,
        "namespace": "blackberry.invoked",
        "dependencies": []
    },
    "io": {
        "global": false,
        "namespace": "blackberry.io",
        "dependencies": []
    },
    "io.filetransfer": {
        "global": false,
        "namespace": "blackberry.io.filetransfer",
        "dependencies": [
            "utils"
        ]
    },
    "jpps": {
        "global": false,
        "namespace": "jpps",
        "dependencies": [
            "utils"
        ]
    },
    "notification": {
        "global": true,
        "namespace": "blackberry.notification",
        "dependencies": []
    },
    "pim.calendar": {
        "global": false,
        "namespace": "blackberry.pim.calendar",
        "dependencies": [
            "utils"
        ]
    },
    "pim.contacts": {
        "global": false,
        "namespace": "blackberry.pim.contacts",
        "dependencies": [
            "utils"
        ]
    },
    "push": {
        "global": false,
        "namespace": "blackberry.push",
        "dependencies": [
            "utils"
        ]
    },
    "sensors": {
        "global": false,
        "namespace": "blackberry.sensors",
        "dependencies": [
            "utils"
        ]
    },
    "system": {
        "global": false,
        "namespace": "blackberry.system",
        "dependencies": [
            "jpps"
        ]
    },
    "ui.contextmenu": {
        "global": false,
        "namespace": "blackberry.ui.contextmenu",
        "dependencies": []
    },
    "ui.cover": {
        "global": false,
        "namespace": "blackberry.ui.cover",
        "dependencies": []
    },
    "ui.dialog": {
        "global": false,
        "namespace": "blackberry.ui.dialog",
        "dependencies": []
    },
    "ui.toast": {
        "global": false,
        "namespace": "blackberry.ui.toast",
        "dependencies": []
    },
    "utils": {
        "global": false,
        "namespace": "utils",
        "dependencies": []
    }
};