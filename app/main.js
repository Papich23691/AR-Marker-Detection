const electron = require('electron');
const url = require('url');
const path = require('path');
var exec = require('child_process').exec;

const { app, BrowserWindow, Menu, ipcMain } = electron;

let mainWindow;
let calWindow;
let status = false;

app.on('ready', function() {
    mainWindow = new BrowserWindow({
        webPreferences: { nodeIntegration: true },
        titleBarStyle: 'hiddenInset',
        resizable: false,
        fullscreen: false
    });
    dir = exec("cd ../ && make", function(err, stdout, stderr) {
        console.log(stdout);
    });
    mainWindow.loadURL(url.format({
        pathname: path.join(__dirname, "index.html"),
        protocol: 'file:',
        slashes: true
    }));
    mainWindow.on('closed', function() {
        dir = exec("ps -ef | grep ./gl | grep -v grep | awk '{print $2}'  | xargs kill ", function(err, stdout, stderr) { console.log(stderr); });
        app.quit();
    });

    const mainMenu = Menu.buildFromTemplate(mainMenuTem);
    Menu.setApplicationMenu(mainMenu);
});

//Open Calibration
function size() {
    calWindow = new BrowserWindow({
        webPreferences: { nodeIntegration: true },
        width: 700,
        resizable: false,
        fullscreen: false,
        height: 250,
        titleBarStyle: 'hiddenInset'
    });
    calWindow.loadURL(url.format({
        pathname: path.join(__dirname, "size.html"),
        protocol: 'file:',
        slashes: true
    }));
    calWindow.on('closed', function() {
        calWindow = null;

    })
}


ipcMain.on('close', function(e, item) {
    if (!status)
        dir = exec("../gl c " + item, function(err, stdout, stderr) {});
    else
        dir = exec("../gl r " + item, function(err, stdout, stderr) {});
    console.log(item);
    calWindow.close();
});

ipcMain.on('marker', function(e, item) {
    dir = exec("ps -ef | grep ./gl | grep -v grep | awk '{print $2}'  | xargs kill ", function(err, stdout, stderr) { console.log(stderr); });
    status = true;
    size();
});

ipcMain.on('calibration', function(e, item) {
    dir = exec("ps -ef | grep ./gl | grep -v grep | awk '{print $2}'  | xargs kill ", function(err, stdout, stderr) { console.log(stderr); });
    status = false;
    size();
});


const mainMenuTem = [{
        label: 'Program',
        submenu: [{
                label: 'Settings',
                accelerator: process.platform == 'darwin' ? 'Cmd+,' : 'Ctrl+,',
            },
            {
                label: 'Developer',
                accelerator: process.platform == 'darwin' ? 'Cmd+I' : 'Ctrl+I',
                click(item, focusedWindow) {
                    focusedWindow.toggleDevTools();
                }

            }

        ]
    },
    {
        label: 'File',
        submenu: [{
                label: 'Calibration',
                accelerator: process.platform == 'darwin' ? 'Option+C' : 'Alt+C',
                click() {
                    dir = exec("ps -ef | grep ./gl | grep -v grep | awk '{print $2}'  | xargs kill ", function(err, stdout, stderr) { console.log(stderr); });
                    status = false;
                    size();
                }
            },
            {
                label: 'Run',
                accelerator: process.platform == 'darwin' ? 'Option+R' : 'Alt+R',
                click() {
                    dir = exec("ps -ef | grep ./gl | grep -v grep | awk '{print $2}'  | xargs kill ", function(err, stdout, stderr) { console.log(stderr); });
                    status = true;
                    size();
                }
            },
            {
                label: 'Quit',
                accelerator: process.platform == 'darwin' ? 'Cmd+Q' : 'Ctrl+Q',
                click() {
                    dir = exec("ps -ef | grep ./gl | grep -v grep | awk '{print $2}'  | xargs kill ", function(err, stdout, stderr) { console.log(stderr); });
                    app.quit();
                }
            }
        ]
    }
];